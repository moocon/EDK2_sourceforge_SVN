/** @file
  Functions declaration related with DHCPv4 for UefiPxeBc Driver.

  Copyright (c) 2009 - 2010, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EFI_PXEBC_DHCP4_H__
#define __EFI_PXEBC_DHCP4_H__

#define PXEBC_DHCP4_OPTION_MAX_NUM         16
#define PXEBC_DHCP4_OPTION_MAX_SIZE        312
#define PXEBC_DHCP4_PACKET_MAX_SIZE        1472
#define PXEBC_DHCP4_S_PORT                 67
#define PXEBC_DHCP4_C_PORT                 68
#define PXEBC_BS_DOWNLOAD_PORT             69
#define PXEBC_BS_DISCOVER_PORT             4011
#define PXEBC_DHCP4_OPCODE_REQUEST         1
#define PXEBC_DHCP4_OPCODE_REPLY           2
#define PXEBC_DHCP4_MSG_TYPE_REQUEST       3
#define PXEBC_DHCP4_MAGIC                  0x63538263 // network byte order

//
// Dhcp Options
//
#define PXEBC_DHCP4_TAG_PAD                0    // Pad Option
#define PXEBC_DHCP4_TAG_EOP                255  // End Option
#define PXEBC_DHCP4_TAG_NETMASK            1    // Subnet Mask
#define PXEBC_DHCP4_TAG_TIME_OFFSET        2    // Time Offset from UTC
#define PXEBC_DHCP4_TAG_ROUTER             3    // Router option,
#define PXEBC_DHCP4_TAG_TIME_SERVER        4    // Time Server
#define PXEBC_DHCP4_TAG_NAME_SERVER        5    // Name Server
#define PXEBC_DHCP4_TAG_DNS_SERVER         6    // Domain Name Server
#define PXEBC_DHCP4_TAG_HOSTNAME           12   // Host Name
#define PXEBC_DHCP4_TAG_BOOTFILE_LEN       13   // Boot File Size
#define PXEBC_DHCP4_TAG_DUMP               14   // Merit Dump File
#define PXEBC_DHCP4_TAG_DOMAINNAME         15   // Domain Name
#define PXEBC_DHCP4_TAG_ROOTPATH           17   // Root path
#define PXEBC_DHCP4_TAG_EXTEND_PATH        18   // Extensions Path
#define PXEBC_DHCP4_TAG_EMTU               22   // Maximum Datagram Reassembly Size
#define PXEBC_DHCP4_TAG_TTL                23   // Default IP Time-to-live
#define PXEBC_DHCP4_TAG_BROADCAST          28   // Broadcast Address
#define PXEBC_DHCP4_TAG_NIS_DOMAIN         40   // Network Information Service Domain
#define PXEBC_DHCP4_TAG_NIS_SERVER         41   // Network Information Servers
#define PXEBC_DHCP4_TAG_NTP_SERVER         42   // Network Time Protocol Servers
#define PXEBC_DHCP4_TAG_VENDOR             43   // Vendor Specific Information
#define PXEBC_DHCP4_TAG_REQUEST_IP         50   // Requested IP Address
#define PXEBC_DHCP4_TAG_LEASE              51   // IP Address Lease Time
#define PXEBC_DHCP4_TAG_OVERLOAD           52   // Option Overload
#define PXEBC_DHCP4_TAG_MSG_TYPE           53   // DHCP Message Type
#define PXEBC_DHCP4_TAG_SERVER_ID          54   // Server Identifier
#define PXEBC_DHCP4_TAG_PARA_LIST          55   // Parameter Request List
#define PXEBC_DHCP4_TAG_MAXMSG             57   // Maximum DHCP Message Size
#define PXEBC_DHCP4_TAG_T1                 58   // Renewal (T1) Time Value
#define PXEBC_DHCP4_TAG_T2                 59   // Rebinding (T2) Time Value
#define PXEBC_DHCP4_TAG_CLASS_ID           60   // Vendor class identifier
#define PXEBC_DHCP4_TAG_CLIENT_ID          61   // Client-identifier
#define PXEBC_DHCP4_TAG_TFTP               66   // TFTP server name
#define PXEBC_DHCP4_TAG_BOOTFILE           67   // Bootfile name
#define PXEBC_PXE_DHCP4_TAG_ARCH           93
#define PXEBC_PXE_DHCP4_TAG_UNDI           94
#define PXEBC_PXE_DHCP4_TAG_UUID           97
//
// Sub-Options in Dhcp Vendor Option
//
#define PXEBC_VENDOR_TAG_MTFTP_IP          1
#define PXEBC_VENDOR_TAG_MTFTP_CPORT       2
#define PXEBC_VENDOR_TAG_MTFTP_SPORT       3
#define PXEBC_VENDOR_TAG_MTFTP_TIMEOUT     4
#define PXEBC_VENDOR_TAG_MTFTP_DELAY       5
#define PXEBC_VENDOR_TAG_DISCOVER_CTRL     6
#define PXEBC_VENDOR_TAG_DISCOVER_MCAST    7
#define PXEBC_VENDOR_TAG_BOOT_SERVERS      8
#define PXEBC_VENDOR_TAG_BOOT_MENU         9
#define PXEBC_VENDOR_TAG_MENU_PROMPT       10
#define PXEBC_VENDOR_TAG_MCAST_ALLOC       11
#define PXEBC_VENDOR_TAG_CREDENTIAL_TYPES  12
#define PXEBC_VENDOR_TAG_BOOT_ITEM         71

#define PXEBC_BOOT_REQUEST_TIMEOUT         1
#define PXEBC_BOOT_REQUEST_RETRIES         4

#define PXEBC_DHCP4_OVERLOAD_FILE          1
#define PXEBC_DHCP4_OVERLOAD_SERVER_NAME   2


//
// The array index of the DHCP4 option tag interested
//
#define PXEBC_DHCP4_TAG_INDEX_BOOTFILE_LEN 0
#define PXEBC_DHCP4_TAG_INDEX_VENDOR       1
#define PXEBC_DHCP4_TAG_INDEX_OVERLOAD     2
#define PXEBC_DHCP4_TAG_INDEX_MSG_TYPE     3
#define PXEBC_DHCP4_TAG_INDEX_SERVER_ID    4
#define PXEBC_DHCP4_TAG_INDEX_CLASS_ID     5
#define PXEBC_DHCP4_TAG_INDEX_BOOTFILE     6
#define PXEBC_DHCP4_TAG_INDEX_MAX          7

//
// Dhcp4 and Dhcp6 share this definition, and corresponding
// relatioinship is as follows:
//
//   Dhcp4Discover <> Dhcp6Solicit
//   Dhcp4Offer    <> Dhcp6Advertise
//   Dhcp4Request  <> Dhcp6Request
//   Dhcp4Ack      <> DHcp6Reply
//
typedef enum {
  PxeOfferTypeDhcpOnly,
  PxeOfferTypeDhcpPxe10,
  PxeOfferTypeDhcpWfm11a,
  PxeOfferTypeDhcpBinl,
  PxeOfferTypeProxyPxe10,
  PxeOfferTypeProxyWfm11a,
  PxeOfferTypeProxyBinl,
  PxeOfferTypeBootp,
  PxeOfferTypeMax
} PXEBC_OFFER_TYPE;

#define BIT(x)                (1 << x)
#define CTRL(x)               (0x1F & (x))
#define DEFAULT_CLASS_ID_DATA "PXEClient:Arch:?????:????:??????"
#define DEFAULT_UNDI_TYPE     1
#define DEFAULT_UNDI_MAJOR    3
#define DEFAULT_UNDI_MINOR    0

#define MTFTP_VENDOR_OPTION_BIT_MAP \
  (BIT (PXEBC_VENDOR_TAG_MTFTP_IP) | \
   BIT (PXEBC_VENDOR_TAG_MTFTP_CPORT) | \
   BIT (PXEBC_VENDOR_TAG_MTFTP_SPORT) | \
   BIT (PXEBC_VENDOR_TAG_MTFTP_TIMEOUT) | \
   BIT (PXEBC_VENDOR_TAG_MTFTP_DELAY))

#define DISCOVER_VENDOR_OPTION_BIT_MAP \
  (BIT (PXEBC_VENDOR_TAG_DISCOVER_CTRL) | \
   BIT (PXEBC_VENDOR_TAG_DISCOVER_MCAST) | \
   BIT (PXEBC_VENDOR_TAG_BOOT_SERVERS) | \
   BIT (PXEBC_VENDOR_TAG_BOOT_MENU) | \
   BIT (PXEBC_VENDOR_TAG_MENU_PROMPT))

#define IS_VALID_BOOT_PROMPT(x) \
  ((((x)[0]) & BIT (PXEBC_VENDOR_TAG_MENU_PROMPT)) \
   == BIT (PXEBC_VENDOR_TAG_MENU_PROMPT))

#define IS_VALID_BOOT_MENU(x) \
  ((((x)[0]) & BIT (PXEBC_VENDOR_TAG_BOOT_MENU)) \
   == BIT (PXEBC_VENDOR_TAG_BOOT_MENU))

#define IS_VALID_MTFTP_VENDOR_OPTION(x) \
  (((UINT32) ((x)[0]) & MTFTP_VENDOR_OPTION_BIT_MAP) \
   == MTFTP_VENDOR_OPTION_BIT_MAP)

#define IS_VALID_DISCOVER_VENDOR_OPTION(x) \
  (((UINT32) ((x)[0]) & DISCOVER_VENDOR_OPTION_BIT_MAP) != 0)

#define IS_VALID_CREDENTIAL_VENDOR_OPTION(x) \
  (((UINT32) ((x)[0]) & BIT (PXEBC_VENDOR_TAG_CREDENTIAL_TYPES)) \
   == BIT (PXEBC_VENDOR_TAG_CREDENTIAL_TYPES))

#define IS_VALID_BOOTITEM_VENDOR_OPTION(x) \
  (((UINT32) ((x)[PXEBC_VENDOR_TAG_BOOT_ITEM / 32]) & \
     BIT (PXEBC_VENDOR_TAG_BOOT_ITEM % 32)) \
    == BIT (PXEBC_VENDOR_TAG_BOOT_ITEM % 32))

#define SET_VENDOR_OPTION_BIT_MAP(x, y) \
  (*(x + ((y) / 32)) = (UINT32) ((UINT32) ((x)[(y) / 32]) | BIT ((y) % 32)))

#define GET_NEXT_DHCP_OPTION(Opt) \
  (EFI_DHCP4_PACKET_OPTION *) ((UINT8 *) (Opt) + \
   sizeof (EFI_DHCP4_PACKET_OPTION) + (Opt)->Length - 1)

#define GET_OPTION_BUFFER_LEN(Pkt) \
  ((Pkt)->Length - sizeof (EFI_DHCP4_HEADER) - 4)

#define GET_NEXT_BOOT_SVR_ENTRY(Ent) \
  (PXEBC_BOOT_SVR_ENTRY *) ((UINT8 *) Ent + sizeof (*(Ent)) + \
   ((Ent)->IpCnt - 1) * sizeof (EFI_IPv4_ADDRESS))

#define IS_PROXY_DHCP_OFFER(Offer) \
  EFI_IP4_EQUAL (&(Offer)->Dhcp4.Header.YourAddr, &mZeroIp4Addr)

#define IS_DISABLE_BCAST_DISCOVER(x) \
  (((x) & BIT (0)) == BIT (0))

#define IS_DISABLE_MCAST_DISCOVER(x) \
  (((x) & BIT (1)) == BIT (1))

#define IS_ENABLE_USE_SERVER_LIST(x) \
  (((x) & BIT (2)) == BIT (2))

#define IS_ENABLE_BOOT_FILE_NAME(x) \
  (((x) & BIT (3)) == BIT (3))


#pragma pack(1)
typedef struct {
  UINT8 ParaList[135];
} PXEBC_DHCP4_OPTION_PARA;

typedef struct {
  UINT16  Size;
} PXEBC_DHCP4_OPTION_MAX_MESG_SIZE;

typedef struct {
  UINT8 Type;
  UINT8 MajorVer;
  UINT8 MinorVer;
} PXEBC_DHCP4_OPTION_UNDI;

typedef struct {
  UINT8 Type;
} PXEBC_DHCP4_OPTION_MESG;

typedef struct {
  UINT16 Type;
} PXEBC_DHCP4_OPTION_ARCH;

typedef struct {
  UINT8 ClassIdentifier[10];
  UINT8 ArchitecturePrefix[5];
  UINT8 ArchitectureType[5];
  UINT8 Lit3[1];
  UINT8 InterfaceName[4];
  UINT8 Lit4[1];
  UINT8 UndiMajor[3];
  UINT8 UndiMinor[3];
} PXEBC_DHCP4_OPTION_CLID;

typedef struct {
  UINT8 Type;
  UINT8 Guid[16];
} PXEBC_DHCP4_OPTION_UUID;

typedef struct {
  UINT16 Type;
  UINT16 Layer;
} PXEBC_OPTION_BOOT_ITEM;

#pragma pack()

typedef union {
  PXEBC_DHCP4_OPTION_PARA           *Para;
  PXEBC_DHCP4_OPTION_UNDI           *Undi;
  PXEBC_DHCP4_OPTION_ARCH           *Arch;
  PXEBC_DHCP4_OPTION_CLID           *Clid;
  PXEBC_DHCP4_OPTION_UUID           *Uuid;
  PXEBC_DHCP4_OPTION_MESG           *Mesg;
  PXEBC_DHCP4_OPTION_MAX_MESG_SIZE  *MaxMesgSize;
} PXEBC_DHCP4_OPTION_ENTRY;

typedef struct {
  UINT16            Type;
  UINT8             IpCnt;
  EFI_IPv4_ADDRESS  IpAddr[1];
} PXEBC_BOOT_SVR_ENTRY;

typedef struct {
  UINT16            Type;
  UINT8             DescLen;
  UINT8             DescStr[1];
} PXEBC_BOOT_MENU_ENTRY;

typedef struct {
  UINT8             Timeout;
  UINT8             Prompt[1];
} PXEBC_MENU_PROMPT;

typedef struct {
  UINT32                BitMap[8];
  EFI_IPv4_ADDRESS      MtftpIp;
  UINT16                MtftpCPort;
  UINT16                MtftpSPort;
  UINT8                 MtftpTimeout;
  UINT8                 MtftpDelay;
  UINT8                 DiscoverCtrl;
  EFI_IPv4_ADDRESS      DiscoverMcastIp;
  EFI_IPv4_ADDRESS      McastIpBase;
  UINT16                McastIpBlock;
  UINT16                McastIpRange;
  UINT16                BootSrvType;
  UINT16                BootSrvLayer;
  PXEBC_BOOT_SVR_ENTRY  *BootSvr;
  UINT8                 BootSvrLen;
  PXEBC_BOOT_MENU_ENTRY *BootMenu;
  UINT8                 BootMenuLen;
  PXEBC_MENU_PROMPT     *MenuPrompt;
  UINT8                 MenuPromptLen;
  UINT32                *CredType;
  UINT8                 CredTypeLen;
} PXEBC_VENDOR_OPTION;

typedef union {
  EFI_DHCP4_PACKET        Offer;
  EFI_DHCP4_PACKET        Ack;
  UINT8                   Buffer[PXEBC_DHCP4_PACKET_MAX_SIZE];
} PXEBC_DHCP4_PACKET;

typedef struct {
  PXEBC_DHCP4_PACKET      Packet;
  PXEBC_OFFER_TYPE        OfferType;
  EFI_DHCP4_PACKET_OPTION *OptList[PXEBC_DHCP4_TAG_INDEX_MAX];
  PXEBC_VENDOR_OPTION     VendorOpt;
} PXEBC_DHCP4_PACKET_CACHE;


/**
  Create a template DHCPv4 packet as a seed.

  @param[out] Seed           Pointer to the seed packet.
  @param[in]  Udp4           Pointer to EFI_UDP4_PROTOCOL.

**/
VOID
PxeBcSeedDhcp4Packet (
  OUT EFI_DHCP4_PACKET       *Seed,
  IN  EFI_UDP4_PROTOCOL      *Udp4
  );


/**
  Parse the cached DHCPv4 packet, including all the options.

  @param[in]  Cache4             Pointer to cached DHCPv4 packet.

  @retval     EFI_SUCCESS        Parsed the DHCPv4 packet successfully.
  @retval     EFI_DEVICE_ERROR   Failed to parse and invalid packet.

**/
EFI_STATUS
PxeBcParseDhcp4Packet (
  IN PXEBC_DHCP4_PACKET_CACHE    *Cache4
  );


/**
  Build and send out the request packet for the bootfile, and parse the reply.

  @param[in]  Private               Pointer to PxeBc private data.
  @param[in]  Type                  PxeBc option boot item type.
  @param[in]  Layer                 Pointer to option boot item layer.
  @param[in]  UseBis                Use BIS or not.
  @param[in]  DestIp                Pointer to the server address.
  @param[in]  IpCount               The total count of the server address.
  @param[in]  SrvList               Pointer to EFI_PXE_BASE_CODE_SRVLIST.

  @retval     EFI_SUCCESS           Successfully discovered boot file.
  @retval     EFI_OUT_OF_RESOURCES  Failed to allocate resource.
  @retval     EFI_NOT_FOUND         Can't get the PXE reply packet.
  @retval     Others                Failed to discover boot file.

**/
EFI_STATUS
PxeBcDhcp4Discover (
  IN  PXEBC_PRIVATE_DATA              *Private,
  IN  UINT16                          Type,
  IN  UINT16                          *Layer,
  IN  BOOLEAN                         UseBis,
  IN  EFI_IP_ADDRESS                  *DestIp,
  IN  UINT16                          IpCount,
  IN  EFI_PXE_BASE_CODE_SRVLIST       *SrvList
  );


/**
  Start the D.O.R.A DHCPv4 process to acquire the IPv4 address and other PXE boot information.

  @param[in]  Private           Pointer to PxeBc private data.
  @param[in]  Dhcp4             Pointer to the EFI_DHCP4_PROTOCOL

  @retval EFI_SUCCESS           The D.O.R.A process successfully finished.
  @retval Others                Failed to finish the D.O.R.A process.

**/
EFI_STATUS
PxeBcDhcp4Dora (
  IN PXEBC_PRIVATE_DATA         *Private,
  IN EFI_DHCP4_PROTOCOL         *Dhcp4
  );

#endif

