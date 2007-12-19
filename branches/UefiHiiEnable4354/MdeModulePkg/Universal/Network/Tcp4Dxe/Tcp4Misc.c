/** @file

Copyright (c) 2005 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Tcp4Misc.c

Abstract:

  Misc support routines for tcp.


**/


#include "Tcp4Main.h"

#include <Library/DevicePathLib.h>

NET_LIST_ENTRY  mTcpRunQue = {
  &mTcpRunQue,
  &mTcpRunQue
};

NET_LIST_ENTRY  mTcpListenQue = {
  &mTcpListenQue,
  &mTcpListenQue
};

TCP_SEQNO       mTcpGlobalIss = 0x4d7e980b;

CHAR16   *mTcpStateName[] = {
  L"TCP_CLOSED",
  L"TCP_LISTEN",
  L"TCP_SYN_SENT",
  L"TCP_SYN_RCVD",
  L"TCP_ESTABLISHED",
  L"TCP_FIN_WAIT_1",
  L"TCP_FIN_WAIT_2",
  L"TCP_CLOSING",
  L"TCP_TIME_WAIT",
  L"TCP_CLOSE_WAIT",
  L"TCP_LAST_ACK"
};


/**
  Initialize the Tcb local related members.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.

  @return None

**/
VOID
TcpInitTcbLocal (
  IN TCP_CB *Tcb
  )
{
  //
  // Compute the checksum of the fixed parts of pseudo header
  //
  Tcb->HeadSum = NetPseudoHeadChecksum (
                  Tcb->LocalEnd.Ip,
                  Tcb->RemoteEnd.Ip,
                  0x06,
                  0
                  );

  Tcb->Iss    = TcpGetIss ();
  Tcb->SndUna = Tcb->Iss;
  Tcb->SndNxt = Tcb->Iss;

  Tcb->SndWl2 = Tcb->Iss;
  Tcb->SndWnd = 536;

  Tcb->RcvWnd = GET_RCV_BUFFSIZE (Tcb->Sk);

  //
  // Fisrt window size is never scaled
  //
  Tcb->RcvWndScale = 0;
}


/**
  Initialize the peer related members.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.
  @param  Seg                   Pointer to the segment that contains the peer's
                                intial info.
  @param  Opt                   Pointer to the options announced by the peer.

  @return None

**/
VOID
TcpInitTcbPeer (
  IN TCP_CB     *Tcb,
  IN TCP_SEG    *Seg,
  IN TCP_OPTION *Opt
  )
{
  UINT16  RcvMss;

  ASSERT (Tcb && Seg && Opt);
  ASSERT (TCP_FLG_ON (Seg->Flag, TCP_FLG_SYN));

  Tcb->SndWnd     = Seg->Wnd;
  Tcb->SndWndMax  = Tcb->SndWnd;
  Tcb->SndWl1     = Seg->Seq;

  if (TCP_FLG_ON (Seg->Flag, TCP_FLG_ACK)) {
    Tcb->SndWl2 = Seg->Ack;
  } else {
    Tcb->SndWl2 = Tcb->Iss + 1;
  }

  if (TCP_FLG_ON (Opt->Flag, TCP_OPTION_RCVD_MSS)) {
    Tcb->SndMss = (UINT16) MAX (64, Opt->Mss);

    RcvMss = TcpGetRcvMss (Tcb->Sk);
    if (Tcb->SndMss > RcvMss) {
      Tcb->SndMss = RcvMss;
    }

  } else {
    //
    // One end doesn't support MSS option, use default.
    //
    Tcb->RcvMss = 536;
  }

  Tcb->CWnd   = Tcb->SndMss;

  Tcb->Irs    = Seg->Seq;
  Tcb->RcvNxt = Tcb->Irs + 1;

  Tcb->RcvWl2 = Tcb->RcvNxt;

  if (TCP_FLG_ON (Opt->Flag, TCP_OPTION_RCVD_WS) &&
      !TCP_FLG_ON (Tcb->CtrlFlag, TCP_CTRL_NO_WS)) {

    Tcb->SndWndScale  = Opt->WndScale;

    Tcb->RcvWndScale  = TcpComputeScale (Tcb);
    TCP_SET_FLG (Tcb->CtrlFlag, TCP_CTRL_RCVD_WS);

  } else {
    //
    // One end doesn't support window scale option. use zero.
    //
    Tcb->RcvWndScale = 0;
  }

  if (TCP_FLG_ON (Opt->Flag, TCP_OPTION_RCVD_TS) &&
      !TCP_FLG_ON (Tcb->CtrlFlag, TCP_CTRL_NO_TS)) {

    TCP_SET_FLG (Tcb->CtrlFlag, TCP_CTRL_SND_TS);
    TCP_SET_FLG (Tcb->CtrlFlag, TCP_CTRL_RCVD_TS);

    //
    // Compute the effective SndMss per RFC1122
    // section 4.2.2.6. If timestamp option is
    // enabled, it will always occupy 12 bytes.
    //
    Tcb->SndMss -= TCP_OPTION_TS_ALIGNED_LEN;
  }
}


/**
  Locate a listen TCB that matchs the Local and Remote.

  @param  Local                 Pointer to the local (IP, Port).
  @param  Remote                Pointer to the remote (IP, Port).

  @return Pointer to the TCP_CB with the least number of wildcard, if NULL no match is found.

**/
STATIC
TCP_CB *
TcpLocateListenTcb (
  IN TCP_PEER *Local,
  IN TCP_PEER *Remote
  )
{
  NET_LIST_ENTRY  *Entry;
  TCP_CB          *Node;
  TCP_CB          *Match;
  INTN            Last;
  INTN            Cur;

  Last  = 4;
  Match = NULL;

  NET_LIST_FOR_EACH (Entry, &mTcpListenQue) {
    Node = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    if ((Local->Port != Node->LocalEnd.Port) ||
        !TCP_PEER_MATCH (Remote, &Node->RemoteEnd) ||
        !TCP_PEER_MATCH (Local, &Node->LocalEnd)
          ) {

      continue;
    }

    //
    // Compute the number of wildcard
    //
    Cur = 0;
    if (Node->RemoteEnd.Ip == 0) {
      Cur++;
    }

    if (Node->RemoteEnd.Port == 0) {
      Cur++;
    }

    if (Node->LocalEnd.Ip == 0) {
      Cur++;
    }

    if (Cur < Last) {
      if (Cur == 0) {
        return Node;
      }

      Last  = Cur;
      Match = Node;
    }
  }

  return Match;
}


/**
  Try to find one Tcb whose <Ip, Port> equals to <IN Addr, IN Port>.

  @param  Addr                  Pointer to the IP address needs to match.
  @param  Port                  The port number needs to match.

  @return The Tcb which matches the <Addr Port> paire exists or not.

**/
BOOLEAN
TcpFindTcbByPeer (
  IN EFI_IPv4_ADDRESS  *Addr,
  IN TCP_PORTNO        Port
  )
{
  TCP_PORTNO      LocalPort;
  NET_LIST_ENTRY  *Entry;
  TCP_CB          *Tcb;

  ASSERT ((Addr != NULL) && (Port != 0));

  LocalPort = HTONS (Port);

  NET_LIST_FOR_EACH (Entry, &mTcpListenQue) {
    Tcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    if (EFI_IP4_EQUAL (Addr, &Tcb->LocalEnd.Ip) &&
      (LocalPort == Tcb->LocalEnd.Port)) {

      return TRUE;
    }
  }

  NET_LIST_FOR_EACH (Entry, &mTcpRunQue) {
    Tcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    if (EFI_IP4_EQUAL (Addr, &Tcb->LocalEnd.Ip) &&
      (LocalPort == Tcb->LocalEnd.Port)) {

      return TRUE;
    }
  }

  return FALSE;
}


/**
  Locate the TCP_CB related to the socket pair.

  @param  LocalPort             The local port number.
  @param  LocalIp               The local IP address.
  @param  RemotePort            The remote port number.
  @param  RemoteIp              The remote IP address.
  @param  Syn                   Whether to search the listen sockets, if TRUE, the
                                listen sockets are searched.

  @return Pointer to the related TCP_CB, if NULL no match is found.

**/
TCP_CB *
TcpLocateTcb (
  IN TCP_PORTNO  LocalPort,
  IN UINT32      LocalIp,
  IN TCP_PORTNO  RemotePort,
  IN UINT32      RemoteIp,
  IN BOOLEAN     Syn
  )
{
  TCP_PEER        Local;
  TCP_PEER        Remote;
  NET_LIST_ENTRY  *Entry;
  TCP_CB          *Tcb;

  Local.Port  = LocalPort;
  Local.Ip    = LocalIp;

  Remote.Port = RemotePort;
  Remote.Ip   = RemoteIp;

  //
  // First check for exact match.
  //
  NET_LIST_FOR_EACH (Entry, &mTcpRunQue) {
    Tcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    if (TCP_PEER_EQUAL (&Remote, &Tcb->RemoteEnd) &&
        TCP_PEER_EQUAL (&Local, &Tcb->LocalEnd)) {

      NetListRemoveEntry (&Tcb->List);
      NetListInsertHead (&mTcpRunQue, &Tcb->List);

      return Tcb;
    }
  }

  //
  // Only check listen queue when SYN flag is on
  //
  if (Syn) {
    return TcpLocateListenTcb (&Local, &Remote);
  }

  return NULL;
}


/**
  Insert a Tcb into the proper queue.

  @param  Tcb                   Pointer to the TCP_CB to be inserted.

  @retval 0                     The Tcb is inserted successfully.
  @retval -1                    Error condition occurred.

**/
INTN
TcpInsertTcb (
  IN TCP_CB *Tcb
  )
{
  NET_LIST_ENTRY   *Entry;
  NET_LIST_ENTRY   *Head;
  TCP_CB           *Node;
  TCP4_PROTO_DATA  *TcpProto;

  ASSERT (
    Tcb &&
    (
    (Tcb->State == TCP_LISTEN) ||
    (Tcb->State == TCP_SYN_SENT) ||
    (Tcb->State == TCP_SYN_RCVD) ||
    (Tcb->State == TCP_CLOSED)
    )
    );

  if (Tcb->LocalEnd.Port == 0) {
    return -1;
  }

  Head = &mTcpRunQue;

  if (Tcb->State == TCP_LISTEN) {
    Head = &mTcpListenQue;
  }

  //
  // Check that Tcb isn't already on the list.
  //
  NET_LIST_FOR_EACH (Entry, Head) {
    Node = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    if (TCP_PEER_EQUAL (&Tcb->LocalEnd, &Node->LocalEnd) &&
        TCP_PEER_EQUAL (&Tcb->RemoteEnd, &Node->RemoteEnd)) {

      return -1;
    }
  }

  NetListInsertHead (Head, &Tcb->List);

  TcpProto = (TCP4_PROTO_DATA *) Tcb->Sk->ProtoReserved;
  TcpSetVariableData (TcpProto->TcpService);

  return 0;
}


/**
  Clone a TCP_CB from Tcb.

  @param  Tcb                   Pointer to the TCP_CB to be cloned.

  @return Pointer to the new cloned TCP_CB, if NULL error condition occurred.

**/
TCP_CB *
TcpCloneTcb (
  IN TCP_CB *Tcb
  )
{
  TCP_CB               *Clone;
  TCP4_SERVICE_DATA  *TcpService;

  Clone = NetAllocatePool (sizeof (TCP_CB));

  if (Clone == NULL) {
    return NULL;

  }

  NetCopyMem (Clone, Tcb, sizeof (TCP_CB));

  //
  // Increate the reference count of the shared IpInfo.
  //
  NET_GET_REF (Tcb->IpInfo);

  NetListInit (&Clone->List);
  NetListInit (&Clone->SndQue);
  NetListInit (&Clone->RcvQue);

  Clone->Sk = SockClone (Tcb->Sk);
  if (Clone->Sk == NULL) {
    TCP4_DEBUG_ERROR (("TcpCloneTcb: failed to clone a sock\n"));
    NetFreePool (Clone);
    return NULL;
  }

  ((TCP4_PROTO_DATA *) (Clone->Sk->ProtoReserved))->TcpPcb = Clone;

  //
  // Open the device path on the handle where service binding resides on.
  //
  TcpService = ((TCP4_PROTO_DATA *) (Clone->Sk->ProtoReserved))->TcpService;
  gBS->OpenProtocol (
         TcpService->ControllerHandle,
         &gEfiDevicePathProtocolGuid,
         (VOID **) &Clone->Sk->ParentDevicePath,
         TcpService->DriverBindingHandle,
         Clone->Sk->SockHandle,
         EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
         );

  return Clone;
}


/**
  Compute an ISS to be used by a new connection.

  None

  @return The result ISS.

**/
TCP_SEQNO
TcpGetIss (
  VOID
  )
{
  mTcpGlobalIss += 2048;
  return mTcpGlobalIss;
}


/**
  Get the local mss.

  None

  @return The mss size.

**/
UINT16
TcpGetRcvMss (
  IN SOCKET  *Sock
  )
{
  EFI_SIMPLE_NETWORK_MODE SnpMode;
  TCP4_PROTO_DATA         *TcpProto;
  EFI_IP4_PROTOCOL        *Ip;

  ASSERT (Sock);

  TcpProto = (TCP4_PROTO_DATA *) Sock->ProtoReserved;
  Ip       = TcpProto->TcpService->IpIo->Ip;
  ASSERT (Ip);

  Ip->GetModeData (Ip, NULL, NULL, &SnpMode);

  return (UINT16) (SnpMode.MaxPacketSize - 40);
}


/**
  Set the Tcb's state.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.
  @param  State                 The state to be set.

  @return None

**/
VOID
TcpSetState (
  IN TCP_CB *Tcb,
  IN UINT8  State
  )
{
  TCP4_DEBUG_TRACE (
    ("Tcb (%x) state %s --> %s\n",
    Tcb,
    mTcpStateName[Tcb->State],
    mTcpStateName[State])
    );

  Tcb->State = State;

  switch (State) {
  case TCP_ESTABLISHED:

    SockConnEstablished (Tcb->Sk);

    if (Tcb->Parent != NULL) {
      //
      // A new connection is accepted by a listening socket, install
      // the device path.
      //
      TcpInstallDevicePath (Tcb->Sk);
    }

    break;

  case TCP_CLOSED:

    SockConnClosed (Tcb->Sk);

    break;
  }
}


/**
  Compute the TCP segment's checksum.

  @param  Nbuf                  Pointer to the buffer that contains the TCP
                                segment.
  @param  HeadSum               The checksum value of the fixed part of pseudo
                                header.

  @return The checksum value.

**/
UINT16
TcpChecksum (
  IN NET_BUF *Nbuf,
  IN UINT16  HeadSum
  )
{
  UINT16  Checksum;

  Checksum  = NetbufChecksum (Nbuf);
  Checksum  = NetAddChecksum (Checksum, HeadSum);

  Checksum = NetAddChecksum (
              Checksum,
              HTONS ((UINT16) Nbuf->TotalSize)
              );

  return (UINT16) ~Checksum;
}


/**
  Translate the information from the head of the received TCP
  segment Nbuf contains and fill it into a TCP_SEG structure.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.
  @param  Nbuf                  Pointer to the buffer contains the TCP segment.

  @return Pointer to the TCP_SEG that contains the translated TCP head information.

**/
TCP_SEG *
TcpFormatNetbuf (
  IN TCP_CB  *Tcb,
  IN NET_BUF *Nbuf
  )
{
  TCP_SEG   *Seg;
  TCP_HEAD  *Head;

  Seg       = TCPSEG_NETBUF (Nbuf);
  Head      = (TCP_HEAD *) NetbufGetByte (Nbuf, 0, NULL);
  Nbuf->Tcp = Head;

  Seg->Seq  = NTOHL (Head->Seq);
  Seg->Ack  = NTOHL (Head->Ack);
  Seg->End  = Seg->Seq + (Nbuf->TotalSize - (Head->HeadLen << 2));

  Seg->Urg  = NTOHS (Head->Urg);
  Seg->Wnd  = (NTOHS (Head->Wnd) << Tcb->SndWndScale);
  Seg->Flag = Head->Flag;

  //
  // SYN and FIN flag occupy one sequence space each.
  //
  if (TCP_FLG_ON (Seg->Flag, TCP_FLG_SYN)) {
    //
    // RFC requires that initial window not be scaled
    //
    Seg->Wnd = NTOHS (Head->Wnd);
    Seg->End++;
  }

  if (TCP_FLG_ON (Seg->Flag, TCP_FLG_FIN)) {
    Seg->End++;
  }

  return Seg;
}


/**
  Reset the connection related with Tcb.

  @param  Tcb                   Pointer to the TCP_CB of the connection to be
                                reset.

  @return None

**/
VOID
TcpResetConnection (
  IN TCP_CB *Tcb
  )
{
  NET_BUF   *Nbuf;
  TCP_HEAD  *Nhead;

  Nbuf = NetbufAlloc (TCP_MAX_HEAD);

  if (Nbuf == NULL) {
    return ;
  }

  Nhead = (TCP_HEAD *) NetbufAllocSpace (
                        Nbuf,
                        sizeof (TCP_HEAD),
                        NET_BUF_TAIL
                        );

  ASSERT (Nhead != NULL);

  Nbuf->Tcp       = Nhead;

  Nhead->Flag     = TCP_FLG_RST;
  Nhead->Seq      = HTONL (Tcb->SndNxt);
  Nhead->Ack      = HTONL (Tcb->RcvNxt);
  Nhead->SrcPort  = Tcb->LocalEnd.Port;
  Nhead->DstPort  = Tcb->RemoteEnd.Port;
  Nhead->HeadLen  = (sizeof (TCP_HEAD) >> 2);
  Nhead->Res      = 0;
  Nhead->Wnd      = HTONS (0xFFFF);
  Nhead->Checksum = 0;
  Nhead->Urg      = 0;
  Nhead->Checksum = TcpChecksum (Nbuf, Tcb->HeadSum);

  TcpSendIpPacket (Tcb, Nbuf, Tcb->LocalEnd.Ip, Tcb->RemoteEnd.Ip);

  NetbufFree (Nbuf);
}


/**
  Initialize an active connection,

  @param  Tcb                   Pointer to the TCP_CB that wants to initiate a
                                connection.

  @return None

**/
VOID
TcpOnAppConnect (
  IN TCP_CB  *Tcb
  )
{
  TcpInitTcbLocal (Tcb);
  TcpSetState (Tcb, TCP_SYN_SENT);

  TcpSetTimer (Tcb, TCP_TIMER_CONNECT, Tcb->ConnectTimeout);
  TcpToSendData (Tcb, 1);
}


/**
  Initiate the connection close procedure, called when
  applications want to close the connection.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.

  @return None.

**/
VOID
TcpOnAppClose (
  IN TCP_CB *Tcb
  )
{
  ASSERT (Tcb);

  if (!NetListIsEmpty (&Tcb->RcvQue) || GET_RCV_DATASIZE (Tcb->Sk)) {

    TCP4_DEBUG_WARN (("TcpOnAppClose: connection reset "
      "because data is lost for TCB %x\n", Tcb));

    TcpResetConnection (Tcb);
    TcpClose (Tcb);
    return;
  }

  switch (Tcb->State) {
  case TCP_CLOSED:
  case TCP_LISTEN:
  case TCP_SYN_SENT:
    TcpSetState (Tcb, TCP_CLOSED);
    break;

  case TCP_SYN_RCVD:
  case TCP_ESTABLISHED:
    TcpSetState (Tcb, TCP_FIN_WAIT_1);
    break;

  case TCP_CLOSE_WAIT:
    TcpSetState (Tcb, TCP_LAST_ACK);
    break;
  }

  TcpToSendData (Tcb, 1);
}


/**
  Check whether the application's newly delivered data
  can be sent out.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.

  @retval 0                     Whether the data is sent out or is buffered for
                                further sending.
  @retval -1                    The Tcb is not in a state that data is permitted to
                                be sent out.

**/
INTN
TcpOnAppSend (
  IN TCP_CB *Tcb
  )
{

  switch (Tcb->State) {
  case TCP_CLOSED:
    return -1;
    break;

  case TCP_LISTEN:
    return -1;
    break;

  case TCP_SYN_SENT:
  case TCP_SYN_RCVD:
    return 0;
    break;

  case TCP_ESTABLISHED:
  case TCP_CLOSE_WAIT:
    TcpToSendData (Tcb, 0);
    return 0;
    break;

  case TCP_FIN_WAIT_1:
  case TCP_FIN_WAIT_2:
  case TCP_CLOSING:
  case TCP_LAST_ACK:
  case TCP_TIME_WAIT:
    return -1;
    break;
  }

  return 0;
}


/**
  Application has consumed some data, check whether
  to send a window updata ack or a delayed ack.

  @param  Tcb                   Pointer to the TCP_CB of this TCP instance.


**/
INTN
TcpOnAppConsume (
  IN TCP_CB *Tcb
  )
{
  UINT32 TcpOld;

  switch (Tcb->State) {
  case TCP_CLOSED:
    return -1;
    break;

  case TCP_LISTEN:
    return -1;
    break;

  case TCP_SYN_SENT:
  case TCP_SYN_RCVD:
    return 0;
    break;

  case TCP_ESTABLISHED:
    TcpOld = TcpRcvWinOld (Tcb);
    if (TcpRcvWinNow (Tcb) > TcpOld) {

      if (TcpOld < Tcb->RcvMss) {

        TCP4_DEBUG_TRACE (("TcpOnAppConsume: send a window"
          " update for a window closed Tcb(%x)\n", Tcb));

        TcpSendAck (Tcb);
      } else if (Tcb->DelayedAck == 0) {

        TCP4_DEBUG_TRACE (("TcpOnAppConsume: scheduled a delayed"
          " ACK to update window for Tcb(%x)\n", Tcb));

        Tcb->DelayedAck = 1;
      }
    }

    break;

  case TCP_CLOSE_WAIT:
    return 0;
    break;

  case TCP_FIN_WAIT_1:
  case TCP_FIN_WAIT_2:
  case TCP_CLOSING:
  case TCP_LAST_ACK:
  case TCP_TIME_WAIT:
    return -1;
    break;
  }

  return -1;
}


/**
  Abort the connection by sending a reset segment, called
  when the application wants to abort the connection.

  @param  Tcb                   Pointer to the TCP_CB of the TCP instance.

  @return None.

**/
VOID
TcpOnAppAbort (
  IN TCP_CB *Tcb
  )
{
  TCP4_DEBUG_WARN (("TcpOnAppAbort: connection reset "
    "issued by application for TCB %x\n", Tcb));

  switch (Tcb->State) {
  case TCP_SYN_RCVD:
  case TCP_ESTABLISHED:
  case TCP_FIN_WAIT_1:
  case TCP_FIN_WAIT_2:
  case TCP_CLOSE_WAIT:
    TcpResetConnection (Tcb);
    break;
  }

  TcpSetState (Tcb, TCP_CLOSED);
}


/**
  Set the Tdp4 variable data.

  @param  Tcp4Service           Tcp4 service data.

  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to set the variable.
  @retval other                 Set variable failed.

**/
EFI_STATUS
TcpSetVariableData (
  IN TCP4_SERVICE_DATA  *Tcp4Service
  )
{
  UINT32                  NumConfiguredInstance;
  NET_LIST_ENTRY          *Entry;
  TCP_CB                  *TcpPcb;
  TCP4_PROTO_DATA         *TcpProto;
  UINTN                   VariableDataSize;
  EFI_TCP4_VARIABLE_DATA  *Tcp4VariableData;
  EFI_TCP4_SERVICE_POINT  *Tcp4ServicePoint;
  CHAR16                  *NewMacString;
  EFI_STATUS              Status;

  NumConfiguredInstance = 0;

  //
  // Go through the running queue to count the instances.
  //
  NET_LIST_FOR_EACH (Entry, &mTcpRunQue) {
    TcpPcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    TcpProto = (TCP4_PROTO_DATA *) TcpPcb->Sk->ProtoReserved;

    if (TcpProto->TcpService == Tcp4Service) {
      //
      // This tcp instance belongs to the Tcp4Service.
      //
      NumConfiguredInstance++;
    }
  }

  //
  // Go through the listening queue to count the instances.
  //
  NET_LIST_FOR_EACH (Entry, &mTcpListenQue) {
    TcpPcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    TcpProto = (TCP4_PROTO_DATA *) TcpPcb->Sk->ProtoReserved;

    if (TcpProto->TcpService == Tcp4Service) {
      //
      // This tcp instance belongs to the Tcp4Service.
      //
      NumConfiguredInstance++;
    }
  }

  //
  // Calculate the size of the Tcp4VariableData. As there may be no Tcp4 child,
  // we should add extra buffer for the service points only if the number of configured
  // children is more than 1.
  //
  VariableDataSize = sizeof (EFI_TCP4_VARIABLE_DATA);

  if (NumConfiguredInstance > 1) {
    VariableDataSize += sizeof (EFI_TCP4_SERVICE_POINT) * (NumConfiguredInstance - 1);
  }

  Tcp4VariableData = NetAllocatePool (VariableDataSize);
  if (Tcp4VariableData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Tcp4VariableData->DriverHandle = Tcp4Service->DriverBindingHandle;
  Tcp4VariableData->ServiceCount = NumConfiguredInstance;

  Tcp4ServicePoint = &Tcp4VariableData->Services[0];

  //
  // Go through the running queue to fill the service points.
  //
  NET_LIST_FOR_EACH (Entry, &mTcpRunQue) {
    TcpPcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    TcpProto = (TCP4_PROTO_DATA *) TcpPcb->Sk->ProtoReserved;

    if (TcpProto->TcpService == Tcp4Service) {
      //
      // This tcp instance belongs to the Tcp4Service.
      //
      Tcp4ServicePoint->InstanceHandle          = TcpPcb->Sk->SockHandle;
      NetCopyMem (&Tcp4ServicePoint->LocalAddress, &TcpPcb->LocalEnd.Ip, sizeof (EFI_IPv4_ADDRESS));
      Tcp4ServicePoint->LocalPort               = NTOHS (TcpPcb->LocalEnd.Port);
      NetCopyMem (&Tcp4ServicePoint->RemoteAddress, &TcpPcb->RemoteEnd.Ip, sizeof (EFI_IPv4_ADDRESS));
      Tcp4ServicePoint->RemotePort              = NTOHS (TcpPcb->RemoteEnd.Port);

      Tcp4ServicePoint++;
    }
  }

  //
  // Go through the listening queue to fill the service points.
  //
  NET_LIST_FOR_EACH (Entry, &mTcpListenQue) {
    TcpPcb = NET_LIST_USER_STRUCT (Entry, TCP_CB, List);

    TcpProto = (TCP4_PROTO_DATA *) TcpPcb->Sk->ProtoReserved;

    if (TcpProto->TcpService == Tcp4Service) {
      //
      // This tcp instance belongs to the Tcp4Service.
      //
      Tcp4ServicePoint->InstanceHandle          = TcpPcb->Sk->SockHandle;
      NetCopyMem (&Tcp4ServicePoint->LocalAddress, &TcpPcb->LocalEnd.Ip, sizeof (EFI_IPv4_ADDRESS));
      Tcp4ServicePoint->LocalPort               = NTOHS (TcpPcb->LocalEnd.Port);
      NetCopyMem (&Tcp4ServicePoint->RemoteAddress, &TcpPcb->RemoteEnd.Ip, sizeof (EFI_IPv4_ADDRESS));
      Tcp4ServicePoint->RemotePort              = NTOHS (TcpPcb->RemoteEnd.Port);

      Tcp4ServicePoint++;
    }
  }

  //
  // Get the mac string.
  //
  Status = NetLibGetMacString (
             Tcp4Service->ControllerHandle,
             Tcp4Service->DriverBindingHandle,
             &NewMacString
             );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  if (Tcp4Service->MacString != NULL) {
    //
    // The variable is set already, we're going to update it.
    //
    if (StrCmp (Tcp4Service->MacString, NewMacString) != 0) {
      //
      // The mac address is changed, delete the previous variable first.
      //
      gRT->SetVariable (
             Tcp4Service->MacString,
             &gEfiTcp4ServiceBindingProtocolGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS,
             0,
             NULL
             );
    }

    NetFreePool (Tcp4Service->MacString);
  }

  Tcp4Service->MacString = NewMacString;

  Status = gRT->SetVariable (
                  Tcp4Service->MacString,
                  &gEfiTcp4ServiceBindingProtocolGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableDataSize,
                  (VOID *) Tcp4VariableData
                  );

ON_ERROR:

  NetFreePool (Tcp4VariableData);

  return Status;
}


/**
  Clear the variable and free the resource.

  @param  Tcp4Service           Tcp4 service data.

  @return None.

**/
VOID
TcpClearVariableData (
  IN TCP4_SERVICE_DATA  *Tcp4Service
  )
{
  ASSERT (Tcp4Service->MacString != NULL);

  gRT->SetVariable (
         Tcp4Service->MacString,
         &gEfiTcp4ServiceBindingProtocolGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         0,
         NULL
         );

  NetFreePool (Tcp4Service->MacString);
  Tcp4Service->MacString = NULL;
}

EFI_STATUS
TcpInstallDevicePath (
  IN SOCKET *Sock
  )
/*++

Routine Description:

  Install the device path protocol on the TCP instance.

Arguments:

  Sock - Pointer to the socket representing the TCP instance.

Returns:

  EFI_SUCCESS - The device path protocol is installed.
  other       - Failed to install the device path protocol.

--*/
{
  TCP4_PROTO_DATA    *TcpProto;
  TCP4_SERVICE_DATA  *TcpService;
  TCP_CB             *Tcb;
  IPv4_DEVICE_PATH   Ip4DPathNode;
  EFI_STATUS         Status;

  TcpProto   = (TCP4_PROTO_DATA *) Sock->ProtoReserved;
  TcpService = TcpProto->TcpService;
  Tcb        = TcpProto->TcpPcb;

  NetLibCreateIPv4DPathNode (
    &Ip4DPathNode,
    TcpService->ControllerHandle,
    Tcb->LocalEnd.Ip,
    NTOHS (Tcb->LocalEnd.Port),
    Tcb->RemoteEnd.Ip,
    NTOHS (Tcb->RemoteEnd.Port),
    EFI_IP_PROTO_TCP,
    Tcb->UseDefaultAddr
    );

  Sock->DevicePath = AppendDevicePathNode (
                     Sock->ParentDevicePath,
                     (EFI_DEVICE_PATH_PROTOCOL *) &Ip4DPathNode
                     );
  if (Sock->DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gBS->InstallProtocolInterface (
                  &Sock->SockHandle,
                  &gEfiDevicePathProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  Sock->DevicePath
                  );
  if (EFI_ERROR (Status)) {
    NetFreePool (Sock->DevicePath);
  }

  return Status;
}
