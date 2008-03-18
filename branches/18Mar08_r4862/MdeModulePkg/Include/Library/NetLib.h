/** @file

Copyright (c) 2005 - 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  NetLib.h

Abstract:

  Library for the UEFI network stack.


**/

#ifndef _NET_LIB_H_
#define _NET_LIB_H_

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/DriverConfiguration.h>
#include <Protocol/DriverDiagnostics.h>
#include <Protocol/Dpc.h>

typedef UINT32          IP4_ADDR;
typedef UINT32          TCP_SEQNO;
typedef UINT16          TCP_PORTNO;

enum {
  NET_ETHER_ADDR_LEN    = 6,
  NET_IFTYPE_ETHERNET   = 0x01,

  EFI_IP_PROTO_UDP      = 0x11,
  EFI_IP_PROTO_TCP      = 0x06,
  EFI_IP_PROTO_ICMP     = 0x01,

  //
  // The address classfication
  //
  IP4_ADDR_CLASSA       = 1,
  IP4_ADDR_CLASSB,
  IP4_ADDR_CLASSC,
  IP4_ADDR_CLASSD,
  IP4_ADDR_CLASSE,

  IP4_MASK_NUM          = 33,
};

#pragma pack(1)

//
// Ethernet head definition
//
typedef struct {
  UINT8                 DstMac [NET_ETHER_ADDR_LEN];
  UINT8                 SrcMac [NET_ETHER_ADDR_LEN];
  UINT16                EtherType;
} ETHER_HEAD;


//
// The EFI_IP4_HEADER is hard to use because the source and
// destination address are defined as EFI_IPv4_ADDRESS, which
// is a structure. Two structures can't be compared or masked
// directly. This is why there is an internal representation.
//
typedef struct {
  UINT8                 HeadLen : 4;
  UINT8                 Ver     : 4;
  UINT8                 Tos;
  UINT16                TotalLen;
  UINT16                Id;
  UINT16                Fragment;
  UINT8                 Ttl;
  UINT8                 Protocol;
  UINT16                Checksum;
  IP4_ADDR              Src;
  IP4_ADDR              Dst;
} IP4_HEAD;


//
// ICMP head definition. ICMP message is categoried as either an error
// message or query message. Two message types have their own head format.
//
typedef struct {
  UINT8                 Type;
  UINT8                 Code;
  UINT16                Checksum;
} IP4_ICMP_HEAD;

typedef struct {
  IP4_ICMP_HEAD         Head;
  UINT32                Fourth; // 4th filed of the head, it depends on Type.
  IP4_HEAD              IpHead;
} IP4_ICMP_ERROR_HEAD;

typedef struct {
  IP4_ICMP_HEAD         Head;
  UINT16                Id;
  UINT16                Seq;
} IP4_ICMP_QUERY_HEAD;


//
// UDP header definition
//
typedef struct {
  UINT16                SrcPort;
  UINT16                DstPort;
  UINT16                Length;
  UINT16                Checksum;
} EFI_UDP4_HEADER;


//
// TCP header definition
//
typedef struct {
  TCP_PORTNO            SrcPort;
  TCP_PORTNO            DstPort;
  TCP_SEQNO             Seq;
  TCP_SEQNO             Ack;
  UINT8                 Res     : 4;
  UINT8                 HeadLen : 4;
  UINT8                 Flag;
  UINT16                Wnd;
  UINT16                Checksum;
  UINT16                Urg;
} TCP_HEAD;

#pragma pack()

#define NET_MAC_EQUAL(pMac1, pMac2, Len)     \
    (CompareMem ((pMac1), (pMac2), Len) == 0)

#define NET_MAC_IS_MULTICAST(Mac, BMac, Len) \
    (((*((UINT8 *) Mac) & 0x01) == 0x01) && (!NET_MAC_EQUAL (Mac, BMac, Len)))

#define NTOHL(x) (UINT32)((((UINT32) (x) & 0xff)     << 24) | \
                          (((UINT32) (x) & 0xff00)   << 8)  | \
                          (((UINT32) (x) & 0xff0000) >> 8)  | \
                          (((UINT32) (x) & 0xff000000) >> 24))

#define HTONL(x)  NTOHL(x)

#define NTOHS(x)  (UINT16)((((UINT16) (x) & 0xff) << 8) | \
                           (((UINT16) (x) & 0xff00) >> 8))

#define HTONS(x)  NTOHS(x)

//
// Test the IP's attribute, All the IPs are in host byte order.
//
#define IP4_IS_MULTICAST(Ip)              (((Ip) & 0xF0000000) == 0xE0000000)
#define IP4_IS_LOCAL_BROADCAST(Ip)        ((Ip) == 0xFFFFFFFF)
#define IP4_NET_EQUAL(Ip1, Ip2, NetMask)  (((Ip1) & (NetMask)) == ((Ip2) & (NetMask)))
#define IP4_IS_VALID_NETMASK(Ip)          (NetGetMaskLength (Ip) != IP4_MASK_NUM)

//
// Convert the EFI_IP4_ADDRESS to plain UINT32 IP4 address.
//
#define EFI_IP4(EfiIpAddr)       (*(IP4_ADDR *) ((EfiIpAddr).Addr))
#define EFI_NTOHL(EfiIp)         (NTOHL (EFI_IP4 ((EfiIp))))
#define EFI_IP4_EQUAL(Ip1, Ip2)  (CompareMem ((Ip1), (Ip2), sizeof (EFI_IPv4_ADDRESS)) == 0)

INTN
NetGetMaskLength (
  IN IP4_ADDR               Mask
  );

INTN
NetGetIpClass (
  IN IP4_ADDR               Addr
  );

BOOLEAN
Ip4IsUnicast (
  IN IP4_ADDR               Ip,
  IN IP4_ADDR               NetMask
  );

extern IP4_ADDR mIp4AllMasks [IP4_MASK_NUM];


extern EFI_IPv4_ADDRESS  mZeroIp4Addr;

#define NET_IS_DIGIT(Ch)            (('0' <= (Ch)) && ((Ch) <= '9'))
#define NET_ROUNDUP(size, unit)     (((size) + (unit) - 1) & (~((unit) - 1)))
#define NET_IS_LOWER_CASE_CHAR(Ch)  (('a' <= (Ch)) && ((Ch) <= 'z'))
#define NET_IS_UPPER_CASE_CHAR(Ch)  (('A' <= (Ch)) && ((Ch) <= 'Z'))

#define TICKS_PER_MS            10000U
#define TICKS_PER_SECOND        10000000U

#define NET_RANDOM(Seed)        ((UINT32) ((UINT32) (Seed) * 1103515245UL + 12345) % 4294967295UL)


UINT32
NetGetUint32 (
  IN UINT8                  *Buf
  );

VOID
NetPutUint32 (
  IN UINT8                  *Buf,
  IN UINT32                 Data
  );

UINT32
NetRandomInitSeed (
  VOID
  );


#define NET_LIST_USER_STRUCT(Entry, Type, Field)        \
          _CR(Entry, Type, Field)

#define NET_LIST_USER_STRUCT_S(Entry, Type, Field, Sig)  \
          CR(Entry, Type, Field, Sig)

//
// Iterate through the doule linked list. It is NOT delete safe
//
#define NET_LIST_FOR_EACH(Entry, ListHead) \
  for(Entry = (ListHead)->ForwardLink; Entry != (ListHead); Entry = Entry->ForwardLink)

//
// Iterate through the doule linked list. This is delete-safe.
// Don't touch NextEntry. Also, don't use this macro if list
// entries other than the Entry may be deleted when processing
// the current Entry.
//
#define NET_LIST_FOR_EACH_SAFE(Entry, NextEntry, ListHead) \
  for(Entry = (ListHead)->ForwardLink, NextEntry = Entry->ForwardLink; \
      Entry != (ListHead); \
      Entry = NextEntry, NextEntry = Entry->ForwardLink \
     )

//
// Make sure the list isn't empty before get the frist/last record.
//
#define NET_LIST_HEAD(ListHead, Type, Field)  \
          NET_LIST_USER_STRUCT((ListHead)->ForwardLink, Type, Field)

#define NET_LIST_TAIL(ListHead, Type, Field)  \
          NET_LIST_USER_STRUCT((ListHead)->BackLink, Type, Field)


LIST_ENTRY *
NetListRemoveHead (
  LIST_ENTRY            *Head
  );

LIST_ENTRY *
NetListRemoveTail (
  LIST_ENTRY            *Head
  );

VOID
NetListInsertAfter (
  IN LIST_ENTRY         *PrevEntry,
  IN LIST_ENTRY         *NewEntry
  );

VOID
NetListInsertBefore (
  IN LIST_ENTRY         *PostEntry,
  IN LIST_ENTRY         *NewEntry
  );


//
// Object container: EFI network stack spec defines various kinds of
// tokens. The drivers can share code to manage those objects.
//
typedef struct {
  LIST_ENTRY                Link;
  VOID                      *Key;
  VOID                      *Value;
} NET_MAP_ITEM;

typedef struct {
  LIST_ENTRY                Used;
  LIST_ENTRY                Recycled;
  UINTN                     Count;
} NET_MAP;

#define NET_MAP_INCREAMENT  64

VOID
NetMapInit (
  IN NET_MAP                *Map
  );

VOID
NetMapClean (
  IN NET_MAP                *Map
  );

BOOLEAN
NetMapIsEmpty (
  IN NET_MAP                *Map
  );

UINTN
NetMapGetCount (
  IN NET_MAP                *Map
  );

EFI_STATUS
NetMapInsertHead (
  IN NET_MAP                *Map,
  IN VOID                   *Key,
  IN VOID                   *Value    OPTIONAL
  );

EFI_STATUS
NetMapInsertTail (
  IN NET_MAP                *Map,
  IN VOID                   *Key,
  IN VOID                   *Value    OPTIONAL
  );

NET_MAP_ITEM  *
NetMapFindKey (
  IN  NET_MAP               *Map,
  IN  VOID                  *Key
  );

VOID *
NetMapRemoveItem (
  IN  NET_MAP               *Map,
  IN  NET_MAP_ITEM          *Item,
  OUT VOID                  **Value   OPTIONAL
  );

VOID *
NetMapRemoveHead (
  IN  NET_MAP               *Map,
  OUT VOID                  **Value   OPTIONAL
  );

VOID *
NetMapRemoveTail (
  IN  NET_MAP               *Map,
  OUT VOID                  **Value OPTIONAL
  );

typedef
EFI_STATUS
(*NET_MAP_CALLBACK) (
  IN NET_MAP                *Map,
  IN NET_MAP_ITEM           *Item,
  IN VOID                   *Arg
  );

EFI_STATUS
NetMapIterate (
  IN NET_MAP                *Map,
  IN NET_MAP_CALLBACK       CallBack,
  IN VOID                   *Arg      OPTIONAL
  );


//
// Helper functions to implement driver binding and service binding protocols.
//
EFI_STATUS
NetLibCreateServiceChild (
  IN  EFI_HANDLE            ControllerHandle,
  IN  EFI_HANDLE            ImageHandle,
  IN  EFI_GUID              *ServiceBindingGuid,
  OUT EFI_HANDLE            *ChildHandle
  );

EFI_STATUS
NetLibDestroyServiceChild (
  IN  EFI_HANDLE            ControllerHandle,
  IN  EFI_HANDLE            ImageHandle,
  IN  EFI_GUID              *ServiceBindingGuid,
  IN  EFI_HANDLE            ChildHandle
  );

EFI_STATUS
NetLibGetMacString (
  IN           EFI_HANDLE  SnpHandle,
  IN           EFI_HANDLE  ImageHandle,
  IN OUT       CHAR16      **MacString
  );

VOID
NetLibCreateIPv4DPathNode (
  IN OUT IPv4_DEVICE_PATH  *Node,
  IN EFI_HANDLE            Controller,
  IN IP4_ADDR              LocalIp,
  IN UINT16                LocalPort,
  IN IP4_ADDR              RemoteIp,
  IN UINT16                RemotePort,
  IN UINT16                Protocol,
  IN BOOLEAN               UseDefaultAddress
  );

EFI_HANDLE
NetLibGetNicHandle (
  IN EFI_HANDLE             Controller,
  IN EFI_GUID               *ProtocolGuid
  );

EFI_STATUS
NetLibQueueDpc (
  IN EFI_TPL            DpcTpl,
  IN EFI_DPC_PROCEDURE  DpcProcedure,
  IN VOID               *DpcContext    OPTIONAL
  );

EFI_STATUS
NetLibDispatchDpc (
  VOID
  );

EFI_STATUS
EFIAPI
NetLibDefaultUnload (
  IN EFI_HANDLE             ImageHandle
  );

enum {
  //
  //Various signatures
  //
  NET_BUF_SIGNATURE    = EFI_SIGNATURE_32 ('n', 'b', 'u', 'f'),
  NET_VECTOR_SIGNATURE = EFI_SIGNATURE_32 ('n', 'v', 'e', 'c'),
  NET_QUE_SIGNATURE    = EFI_SIGNATURE_32 ('n', 'b', 'q', 'u'),


  NET_PROTO_DATA       = 64,   // Opaque buffer for protocols
  NET_BUF_HEAD         = 1,    // Trim or allocate space from head
  NET_BUF_TAIL         = 0,    // Trim or allocate space from tail
  NET_VECTOR_OWN_FIRST = 0x01, // We allocated the 1st block in the vector
};

#define NET_CHECK_SIGNATURE(PData, SIGNATURE) \
  ASSERT (((PData) != NULL) && ((PData)->Signature == (SIGNATURE)))

#define NET_SWAP_SHORT(Value) \
  ((((Value) & 0xff) << 8) | (((Value) >> 8) & 0xff))

//
// Single memory block in the vector.
//
typedef struct {
  UINT32              Len;        // The block's length
  UINT8               *Bulk;      // The block's Data
} NET_BLOCK;

typedef VOID (*NET_VECTOR_EXT_FREE) (VOID *Arg);

//
//NET_VECTOR contains several blocks to hold all packet's
//fragments and other house-keeping stuff for sharing. It
//doesn't specify the where actual packet fragment begins.
//
typedef struct {
  UINT32              Signature;
  INTN                RefCnt;  // Reference count to share NET_VECTOR.
  NET_VECTOR_EXT_FREE Free;    // external function to free NET_VECTOR
  VOID                *Arg;    // opeque argument to Free
  UINT32              Flag;    // Flags, NET_VECTOR_OWN_FIRST
  UINT32              Len;     // Total length of the assocated BLOCKs

  UINT32              BlockNum;
  NET_BLOCK           Block[1];
} NET_VECTOR;

//
//NET_BLOCK_OP operate on the NET_BLOCK, It specifies
//where the actual fragment begins and where it ends
//
typedef struct {
  UINT8               *BlockHead;   // Block's head, or the smallest valid Head
  UINT8               *BlockTail;   // Block's tail. BlockTail-BlockHead=block length
  UINT8               *Head;        // 1st byte of the data in the block
  UINT8               *Tail;        // Tail of the data in the block, Tail-Head=Size
  UINT32              Size;         // The size of the data
} NET_BLOCK_OP;


//
//NET_BUF is the buffer manage structure used by the
//network stack. Every network packet may be fragmented,
//and contains multiple fragments. The Vector points to
//memory blocks used by the each fragment, and BlockOp
//specifies where each fragment begins and ends.
//
//It also contains a opaque area for protocol to store
//per-packet informations. Protocol must be caution not
//to overwrite the members after that.
//
typedef struct {
  UINT32              Signature;
  INTN                RefCnt;
  LIST_ENTRY          List;       // The List this NET_BUF is on

  IP4_HEAD            *Ip;        // Network layer header, for fast access
  TCP_HEAD            *Tcp;       // Transport layer header, for fast access
  UINT8               ProtoData [NET_PROTO_DATA]; //Protocol specific data

  NET_VECTOR          *Vector;    // The vector containing the packet

  UINT32              BlockOpNum; // Total number of BlockOp in the buffer
  UINT32              TotalSize;  // Total size of the actual packet
  NET_BLOCK_OP        BlockOp[1]; // Specify the position of actual packet
} NET_BUF;


//
//A queue of NET_BUFs, It is just a thin extension of
//NET_BUF functions.
//
typedef struct {
  UINT32              Signature;
  INTN                RefCnt;
  LIST_ENTRY          List;       // The List this buffer queue is on

  LIST_ENTRY          BufList;    // list of queued buffers
  UINT32              BufSize;    // total length of DATA in the buffers
  UINT32              BufNum;     // total number of buffers on the chain
} NET_BUF_QUEUE;

//
// Pseudo header for TCP and UDP checksum
//
#pragma pack(1)
typedef struct {
  IP4_ADDR            SrcIp;
  IP4_ADDR            DstIp;
  UINT8               Reserved;
  UINT8               Protocol;
  UINT16              Len;
} NET_PSEUDO_HDR;
#pragma pack()

//
// The fragment entry table used in network interfaces. This is
// the same as NET_BLOCK now. Use two different to distinguish
// the two in case that NET_BLOCK be enhanced later.
//
typedef struct {
  UINT32              Len;
  UINT8               *Bulk;
} NET_FRAGMENT;

#define NET_GET_REF(PData)      ((PData)->RefCnt++)
#define NET_PUT_REF(PData)      ((PData)->RefCnt--)
#define NETBUF_FROM_PROTODATA(Info) _CR((Info), NET_BUF, ProtoData)

#define NET_BUF_SHARED(Buf) \
  (((Buf)->RefCnt > 1) || ((Buf)->Vector->RefCnt > 1))

#define NET_VECTOR_SIZE(BlockNum) \
  (sizeof (NET_VECTOR) + ((BlockNum) - 1) * sizeof (NET_BLOCK))

#define NET_BUF_SIZE(BlockOpNum)  \
  (sizeof (NET_BUF) + ((BlockOpNum) - 1) * sizeof (NET_BLOCK_OP))

#define NET_HEADSPACE(BlockOp)  \
  (UINTN)((BlockOp)->Head - (BlockOp)->BlockHead)

#define NET_TAILSPACE(BlockOp)  \
  (UINTN)((BlockOp)->BlockTail - (BlockOp)->Tail)

NET_BUF  *
NetbufAlloc (
  IN UINT32                 Len
  );

VOID
NetbufFree (
  IN NET_BUF                *Nbuf
  );


UINT8  *
NetbufGetByte (
  IN  NET_BUF               *Nbuf,
  IN  UINT32                Offset,
  OUT UINT32                *Index      OPTIONAL
  );

NET_BUF  *
NetbufClone (
  IN NET_BUF                *Nbuf
  );

NET_BUF  *
NetbufDuplicate (
  IN NET_BUF                *Nbuf,
  IN NET_BUF                *Duplicate    OPTIONAL,
  IN UINT32                 HeadSpace
  );

NET_BUF  *
NetbufGetFragment (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Offset,
  IN UINT32                 Len,
  IN UINT32                 HeadSpace
  );

VOID
NetbufReserve (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Len
  );

UINT8  *
NetbufAllocSpace (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Len,
  IN BOOLEAN                FromHead
  );

UINT32
NetbufTrim (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Len,
  IN BOOLEAN                FromHead
  );

UINT32
NetbufCopy (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Offset,
  IN UINT32                 Len,
  IN UINT8                  *Dest
  );

NET_BUF  *
NetbufFromExt (
  IN NET_FRAGMENT           *ExtFragment,
  IN UINT32                 ExtNum,
  IN UINT32                 HeadSpace,
  IN UINT32                 HeadLen,
  IN NET_VECTOR_EXT_FREE    ExtFree,
  IN VOID                   *Arg          OPTIONAL
  );

EFI_STATUS
NetbufBuildExt (
  IN NET_BUF                *Nbuf,
  IN NET_FRAGMENT           *ExtFragment,
  IN UINT32                 *ExtNum
  );

NET_BUF  *
NetbufFromBufList (
  IN LIST_ENTRY             *BufList,
  IN UINT32                 HeadSpace,
  IN UINT32                 HeaderLen,
  IN NET_VECTOR_EXT_FREE    ExtFree,
  IN VOID                   *Arg                OPTIONAL
  );

VOID
NetbufFreeList (
  IN LIST_ENTRY             *Head
  );

VOID
NetbufQueInit (
  IN NET_BUF_QUEUE          *NbufQue
  );

NET_BUF_QUEUE  *
NetbufQueAlloc (
  VOID
  );

VOID
NetbufQueFree (
  IN NET_BUF_QUEUE          *NbufQue
  );

NET_BUF  *
NetbufQueRemove (
  IN NET_BUF_QUEUE          *NbufQue
  );

VOID
NetbufQueAppend (
  IN NET_BUF_QUEUE          *NbufQue,
  IN NET_BUF                *Nbuf
  );

UINT32
NetbufQueCopy (
  IN NET_BUF_QUEUE          *NbufQue,
  IN UINT32                 Offset,
  IN UINT32                 Len,
  IN UINT8                  *Dest
  );

UINT32
NetbufQueTrim (
  IN NET_BUF_QUEUE          *NbufQue,
  IN UINT32                 Len
  );

VOID
NetbufQueFlush (
  IN NET_BUF_QUEUE          *NbufQue
  );

UINT16
NetblockChecksum (
  IN UINT8                  *Bulk,
  IN UINT32                 Len
  );

UINT16
NetAddChecksum (
  IN UINT16                 Checksum1,
  IN UINT16                 Checksum2
  );

UINT16
NetbufChecksum (
  IN NET_BUF                *Nbuf
  );

UINT16
NetPseudoHeadChecksum (
  IN IP4_ADDR               Src,
  IN IP4_ADDR               Dst,
  IN UINT8                  Proto,
  IN UINT16                 Len
  );

#endif
