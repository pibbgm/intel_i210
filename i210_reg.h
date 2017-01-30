#ifndef __I210_REG_H__
#define __I210_REG_H__


#define         CTRL		0x0000	     //Device Control Register RW
#define         STATUS		0x0008	     //Status Register RO
#define         CTRL_EXT	0x0018	     //Extended Device Control Register RW
#define         MDIC		0x0020	     //MDI Control Register RW
#define         FCAL		0x0028	     //Flow Control Address Low RO
#define         FCAH		0x002C	     //Flow Control Address High RO
#define         FCT		0x0030	     //Flow Control Type RW
#define         CONNSW		0x0034	     //Copper/Fiber Switch Control RW
#define         VET		0x0038	     //VLAN Ether Type RW
#define         MDICNFG		0x0E04	     //MDC/MDIO Configuration Register RW
#define         FCTTV		0x0170	     //Flow Control Transmit Timer Value RW
#define         LEDCTL		0x0E00	     //LED Control Register RW
#define         I2CCMD		0x1028	     //SFP I2C Command RW
#define         I2CPARAMS	0x102C	     //SFP I2C Parameter RW
#define         WDSTP		0x1040	     //Watchdog Setup Register RW
#define         WDSWSTS		0x1044	     //Watchdog Software RW
#define         FRTIMER		0x1048	     //Free Running Timer RWM
#define         TCPTimer	0x104C	     //TCP Timer RW
#define         DCA_ID		0x5B70	     //DCA Requester ID Information Register RO
#define         SWSM		0x5B50	     //Software Semaphore Register RW
#define         FWSM		0x5B54	     //Firmware Semaphore Register RWM
#define         SW_FW_SYNC	0x5B5C	     //Software-Firmware Synchronization RWM
#define         IPCNFG		0x0E38	     //Internal PHY Configuration RW
#define         PHPM		0x0E14	     //PHY Power Management RW

/* Flash-Security Registers */
#define         EEC		0x12010      //EEPROM-Mode Control Register RW
#define         EERD		0x12014      //EEPROM-Mode Read Register RW
#define         EEWR		0x12018      //EEPROM-Mode Write Register RW
#define         FLA		0x1201C      //Flash Access Register RW
#define         EEMNGCTL	0x12030      //Manageability EEPROM-Mode Control Register RW
#define         EEARBC		0x12024      //EEPROM Block Auto Read Bus Control RW
#define         FLASHMODE	0x12000      //Flash Mode Register RO
#define         FLASHOP		0x12054      //Flash OP-Code Register RO
#define         FLASHGOP	0x12058      //Flash General Purpose OP-Code Register RO
#define         FLASHTIME	0x12004      //Flash Access Timing Register RO
#define         FLBLKBASE	0x12100      //Flash Block Base Address RO
#define         FLBLKEND	0x12104      //Flash Block End Address RO
#define         FLFWUPDATE	0x12108      //Flash Firmware Code Update RW
#define         EEBLKBASE	0x1210C      //EEPROM Block Base Address RO
#define         EEBLKEND	0x12110      //EEPROM Block End Address RO
#define         FLSWCTL		0x12048      //Software Flash Burst Control Register RW
#define         FLSWDATA	0x1204C      //Software Flash Burst Data Register RW
#define         FLSWCNT		0x12050      //Software Flash Burst Access Counter RW
#define         INVM_DATA(n)	(0x12120+(4*n)) //[0 - 63] iNVM Data Register R/W1
#define         INVM_LOCK	(0x12220+(4*n)) //[0 -31] iNVM Lock Register R/W1
#define         INVM_PROTECT	0x12324      //iNVM Protect Register RW

         /* Interrupts */
#define         ICR		0x1500      //Interrupt Cause Read RC/W1C
#define         ICS		0x1504      //Interrupt Cause Set WO
#define         IMS		0x1508      //Interrupt Mask Set/Read RW
#define         IMC		0x150C      //Interrupt Mask Clear WO
#define         IAM		0x1510      //Interrupt Acknowledge Auto Mask RW
#define         EICS		0x1520      //Extended Interrupt Cause Set WO
#define         EIMS		0x1524      //Extended Interrupt Mask Set/Read RWM
#define         EIMC		0x1528      //Extended Interrupt Mask Clear WO
#define         EIAC		0x152C      //Extended Interrupt Auto Clear RW
#define         EIAM		0x1530      //Extended Interrupt Auto Mask RW
#define         EICR		0x1580      //Extended Interrupt Cause Read RC/W1C

         /* Receive */
#define         RCTL		0x0100      //Rx Control RW
#define         FCRTL0		0x2160      //Flow Control Receive Threshold Low RW
#define         FCRTH0		0x2168      //Flow Control Receive Threshold High RW
#define         RXPBSIZE	0x2404      //Rx Packet Buffer Size RW
#define         FCRTV		0x2460      //Flow Control Refresh Timer Value RW

#define         RDBAL0		0xC000      //Rx Descriptor Base Low Queue 0 RW
#define         RDBAH0		0xC004      //Rx Descriptor Base High Queue 0 RW
#define         RDLEN0		0xC008      //Rx Descriptor Ring Length Queue 0 RW
#define         SRRCTL0		0xC00C      //Split and Replication Receive Control Register Queue 0 RW
#define         RDH0		0xC010      //Rx Descriptor Head Queue 0 RO
#define         RDT0		0xC018      //Rx Descriptor Tail Queue 0 RW
#define         RXDCTL0		0xC028      //Receive Descriptor Control Queue 0 RW
#define         RXCTL0		0xC014      //Receive Queue 0 DCA CTRL Register RW

#define         RDBAL(n)	(0xC000 + 0x40 * (n-1))      //Rx Descriptor Base Low Queue 1 - 3 RW
#define         RDBAH(n)	(0xC004 + 0x40 * (n-1))      //Rx Descriptor Base High Queue 1 - 3 RW
#define         RDLEN(n)	(0xC008 + 0x40 * (n-1))      //Rx Descriptor Ring Length Queue 1 - 3 RW
#define         SRRCTL(n)	(0xC00C + 0x40 * (n-1))      //Split and Replication Receive Control Register Queue 1 - 3 RW
#define         RDH(n)		(0xC010 + 0x40 * (n-1))      //Rx Descriptor Head Queue 1 - 3 RO
#define         RDT(n)		(0xC018 + 0x40 * (n-1))      //Rx Descriptor Tail Queue 1 - 3 RW
#define         RXDCTL(n)	(0xC028 + 0x40 * (n-1))      //Receive Descriptor Control Queue 1 - 3 RW
#define         RXCTL(n)	(0xC014 + 0x40 * (n-1))      //Receive Queue 1 - 3 DCA CTRL Register RW

#if 0
#define         RDBAL1_3(n)	(0xC040 + 0x40 * (n-1))      //Rx Descriptor Base Low Queue 1 - 3 RW
#define         RDBAH1_3(n)	(0xC044 + 0x40 * (n-1))      //Rx Descriptor Base High Queue 1 - 3 RW
#define         RDLEN1_3(n)	(0xC048 + 0x40 * (n-1))      //Rx Descriptor Ring Length Queue 1 - 3 RW
#define         SRRCTL1_3(n)	(0xC04C + 0x40 * (n-1))      //Split and Replication Receive Control Register Queue 1 - 3 RW
#define         RDH1_3(n)	(0xC050 + 0x40 * (n-1))      //Rx Descriptor Head Queue 1 - 3 RO
#define         RDT1_3(n)	(0xC058 + 0x40 * (n-1))      //Rx Descriptor Tail Queue 1 - 3 RW
#define         RXDCTL1_3(n)	(0xC068 + 0x40 * (n-1))      //Receive Descriptor Control Queue 1 - 3 RW
#define         RXCTL1_3(n)	(0xC054 + 0x40 * (n-1))      //Receive Queue 1 - 3 DCA CTRL Register RW
#endif

#define         RXCSUM		0x5000      //Receive Checksum Control RW
#define         RLPML		0x5004      //Receive Long packet maximal length RW
#define         RFCTL		0x5008      //Receive Filter Control Register RW
#define         MTA_127_0(n)	(0x5200 + (4 * (127 - (n))))	//Multicast Table Array (n) RW
#define         RAL_0_15(n)	(0x5400 + (8 * (n)))		//Receive Address Low (15:0) RW
#define         RAH_0_15(n)	(0x5404 + (8 * (n)))		//Receive Address High (15:0) RW
#define         PSRTYPE_3_0(n)	(0x5480 + (4 * (3 - n)))	//Packet Split Receive type (n) RW
#define         VFTA_127_0(n)	(0x5600 + (4 * (127 - (n))))	//VLAN Filter Table Array (n) RW
#define         MRQC		0x5818				//Multiple Receive Queues Command RW
#define         RETA(n)		(0x5C00 + (4 * (n)))		//Redirection Table RW
#define         RSSRK(n)	(0x5C80 + (4 * (n)))   //RSS Random Key Register RW
#define         DVMOLR_0_3(n)	(0xC038 + (0x40*(n))) //DMA VM Offload Register[0-3] RW

		/* Transmit */
#define         TCTL		0x0400       //Tx Control RW
#define         TCTL_EXT	0x0404       //Tx Control Extended RW
#define         TIPG		0x0410       //Tx IPG RW
#define         RETX_CTL	0x041C       //Retry Buffer Control RW
#define         TXPBSIZE	0x3404       //Transmit Packet Buffer Size RW
#define         DTXTCPFLGL	0x359C       //DMA Tx TCP Flags Control Low RW
#define         DTXTCPFLGH	0x35A0       //DMA Tx TCP Flags Control High RW
#define         DTXMXSZRQ	0x3540       //DMA Tx Max Total Allow Size Requests RW
#define         DTXMXPKTSZ	0x355C       //DMA Tx Max Allowable Packet Size RW
#define         DTXCTL		0x3590       //DMA Tx Control RW
#define         DTXBCTL		0x35A4       //DMA Tx Behavior Control RW

#define         TDBAL0		0xE000       //Tx Descriptor Base Low 0 RW
#define         TDBAH0		0xE004       //Tx Descriptor Base High 0 RW
#define         TDLEN0		0xE008       //Tx Descriptor Ring Length 0 RW
#define         TDH0		0xE010       //Tx Descriptor Head 0 RO
#define         TDT0		0xE018       //Tx Descriptor Tail 0 RW
#define         TXDCTL0		0xE028       //Transmit Descriptor Control Queue 0 RW
#define         TXCTL0		0xE014       //Tx DCA CTRL Register Queue 0 RW
#define         TDWBAL0		0xE038       //Transmit Descriptor WB Address Low Queue 0 RW
#define         TDWBAH0		0xE03C       //Transmit Descriptor WB Address High Queue 0 RW

#define         TDBAL(n)	(0xE000 + 0x40 * (n))		//Tx Descriptor Base Low Queue 1 - 3 RW
#define         TDBAH(n)	(0xE004 + 0x40 * (n))		//Tx Descriptor Base High Queue 1 - 3 RW
#define         TDLEN(n)	(0xE008 + 0x40 * (n))		//Tx Descriptor Ring Length Queue 1 - 3 RW
#define         TDH(n)		(0xE010 + 0x40 * (n))		//Tx Descriptor Head Queue 1 - 3 RO
#define         TDT(n)		(0xE018 + 0x40 * (n))		//Tx Descriptor Tail Queue 1 - 3 RW
#define         TXDCTL(n)	(0xE028 + 0x40 * (n))		//Transmit Descriptor Control 1 - 3 RW
#define         TXCTL(n)	(0xE014 + 0x40 * (n))		//Tx DCA CTRL Register Queue 1 - 3 RW
#define         TDWBAL(n)	(0xE038 + 0x40 * (n))		//Transmit Descriptor WB Address Low Queue 1 - 3 RW
#define         TDWBAH(n)	(0xE03C + 0x40 * (n))		//Transmit Descriptor WB Address High Queue 1 - 3 RW

#if 0
#define         TDBAL_1_3(n)	(0xE040 + 0x40 * (n-1))		//Tx Descriptor Base Low Queue 1 - 3 RW
#define         TDBAH_1_3(n)	(0xE044 + 0x40 * (n-1))		//Tx Descriptor Base High Queue 1 - 3 RW
#define         TDLEN_1_3(n)	(0xE048 + 0x40 * (n-1))		//Tx Descriptor Ring Length Queue 1 - 3 RW
#define         TDH_1_3(n)	(0xE050 + 0x40 * (n-1))		//Tx Descriptor Head Queue 1 - 3 RO
#define         TDT_1_3(n)	(0xE058 + 0x40 * (n-1))		//Tx Descriptor Tail Queue 1 - 3 RW
#define         TXDCTL_1_3(n)	(0xE068 + 0x40 * (n-1))		//Transmit Descriptor Control 1 - 3 RW
#define         TXCTL_1_3(n)	(0xE054 + 0x40 * (n-1))		//Tx DCA CTRL Register Queue 1 - 3 RW
#define         TDWBAL_1_3(n)	(0xE078 + 0x40 * (n-1))		//Transmit Descriptor WB Address Low Queue 1 - 3 RW
#define         TDWBAH_1_3(n)	(0xE07C + 0x40 * (n-1))		//Transmit Descriptor WB Address High Queue 1 - 3 RW
#endif

#define         TQAVHC(n)	(0x300C + 0x40 * n)		//Transmit Qav High Credits RW
#define         TQAVCC_0_1(n)	(0x3004 + 0x40 * n)		//Transmit Qav RW
#define         TQAVCTRL	0x3570				//Transmit Qav Control RW

		/* Filters */
#define         ETQF_0_7(n)		(0x5CB0 + 4*n)          //EType Queue Filter 0 - 7 RW
#define         IMIR_0_7(n)		(0x5A80 + 4*n)          //Immediate Interrupt Rx 0 - 7 RW
#define         IMIREXT_0_7(n)		(0x5AA0 + 4*n)          //Immediate Interrupt Rx Extended 0 - 7 RW
#define         IMIRVP			0x5AC0                  //Immediate Interrupt Rx VLAN Priority RW
#define         TTQF_0_7(n)		(0x59E0 + 4*n)          //Two-Tuple Queue Filter 0 - 7 RW
#define         SYNQF			0x55FC                  //SYN Packet Queue Filter RW

	         /* Per Queue Statistics */
#define         RQDPC_0_3(n)		(0xC030 + 0x40 * n)       //Receive Queue Drop Packet Count Register 0 - 3 RW
#define         TQDPC_0_3(n)		(0xE030 + 0x40 * n)       //Transmit Queue Drop Packet Count Register 0 - 3 RW
#define         PQGPRC_0_3(n)		(0x10010 + 0x100*n)       //Per Queue Good Packets Received Count RO
#define         PQGPTC_0_3(n)		(0x10014 + 0x100*n)       //Per Queue Good Packets Transmitted Count RO
#define         PQGORC_0_3(n)		(0x10018 + 0x100*n)       //Per Queue Good Octets Received Count RO
#define         PQGOTC_0_3(n)		(0x10034 + 0x100*n)       //Per Queue Octets Transmitted Count RO
#define         PQMPRC_0_3(n)		(0x1003C + 0x100*n)       //Per Queue Multicast Packets Received Count RO

         /* Statistics */
#define         CRCERRS		0x4000       //CRC Error Count RC
#define         ALGNERRC	0x4004       //Alignment Error Count RC
#define         SYMERRS		0x4008       //Symbol Error Count RC
#define         RXERRC		0x400C       //Rx Error Count RC
#define         MPC		0x4010       //Missed Packets Count RC
#define         SCC		0x4014       //Single Collision Count RC
#define         ECOL		0x4018       //Excessive Collisions Count RC
#define         MCC		0x401C       //Multiple Collision Count RC
#define         LATECOL		0x4020       //Late Collisions Count RC
#define         COLC		0x4028       //Collision Count RC
#define         DC		0x4030       //Defer Count RC
#define         TNCRS		0x4034       //Transmit - No CRS RC
#define         HTDPMC		0x403C       //Host Transmit Discarded Packets by MAC Count RC
#define         RLEC		0x4040       //Receive Length Error Count RC
#define         XONRXC		0x4048       //XON Received Count RC
#define         XONTXC		0x404C       //XON Transmitted Count RC
#define         XOFFRXC		0x4050       //XOFF Received Count RC
#define         XOFFTXC		0x4054       //XOFF Transmitted Count RC
#define         FCRUC		0x4058       //FC Received Unsupported Count RC
#define         PRC64		0x405C       //Packets Received (64 Bytes) Count RC
#define         PRC127		0x4060       //Packets Received (65-127 Bytes) Count RC
#define         PRC255		0x4064       //Packets Received (128-255 Bytes) Count RC
#define         PRC511		0x4068       //Packets Received (256-511 Bytes) Count RC
#define         PRC1023		0x406C       //Packets Received (512-1023 Bytes) Count RC
#define         PRC1522		0x4070       //Packets Received (1024-1522 Bytes) RC
#define         GPRC		0x4074       //Good Packets Received Count RC
#define         BPRC		0x4078       //Broadcast Packets Received Count RC
#define         MPRC		0x407C       //Multicast Packets Received Count RC
#define         GPTC		0x4080       //Good Packets Transmitted Count RC
#define         GORCL		0x4088       //Good Octets Received Count (Lo) RC
#define         GORCH		0x408C       //Good Octets Received Count (Hi) RC
#define         GOTCL		0x4090       //Good Octets Transmitted Count (Lo) RC
#define         GOTCH		0x4094       //Good Octets Transmitted Count (Hi) RC
#define         RNBC		0x40A0       //Receive No Buffers Count RC
#define         RUC		0x40A4       //Receive Under Size Count RC
#define         RFC		0x40A8       //Receive Fragment Count RC
#define         ROC		0x40AC       //Receive Oversize Count RC
#define         RJC		0x40B0       //Receive Jabber Count RC
#define         MNGPRC		0x40B4       //Management Packets Receive Count RC
#define         MPDC		0x40B8       //Management Packets Dropped Count RC
#define         MNGPTC		0x40BC       //Management Packets Transmitted Count RC
#define         TORL		0x40C0       //Total Octets Received (Lo) RC
#define         B2OSPC		0x8FE0       //BMC2OS Packets Sent by MC RC
#define         B2OGPRC		0x4158       //BMC2OS Packets Received by Host RC
#define         O2BGPTC		0x8FE4       //OS2BMC Packets Received by MC RC
#define         O2BSPC		0x415C       //OS2BMC Packets Transmitted By Host RC
#define         TORH		0x40C4       //Total Octets Received (Hi) RC
#define         TOTL		0x40C8       //Total Octets Transmitted (Lo) RC
#define         TOTH		0x40CC       //Total Octets Transmitted (Hi) RC
#define         TPR		0x40D0       //Total Packets Received RC
#define         TPT		0x40D4       //Total Packets Transmitted RC
#define         PTC64		0x40D8       //Packets Transmitted (64 Bytes) Count RC
#define         PTC127		0x40DC       //Packets Transmitted (65-127 Bytes) Count RC
#define         PTC255		0x40E0       //Packets Transmitted (128-256 Bytes) Count RC
#define         PTC511		0x40E4       //Packets Transmitted (256-511 Bytes) Count RC
#define         PTC1023		0x40E8       //Packets Transmitted (512-1023 Bytes) Count RC
#define         PTC1522		0x40EC       //Packets Transmitted (1024-1522 Bytes) Count RC
#define         MPTC		0x40F0       //Multicast Packets Transmitted Count RC
#define         BPTC		0x40F4       //Broadcast Packets Transmitted Count RC
#define         TSCTC		0x40F8       //TCP Segmentation Context Transmitted Count RC
#define         IAC		0x4100       //Interrupt Assertion Count RC
#define         RPTHC		0x4104       //Rx Packets to Host Count RC
#define         TLPIC		0x4148       //EEE Tx LPI Count RC
#define         RLPIC		0x414C       //EEE Rx LPI Count RC
#define         HGPTC		0x4118       //Host Good Packets Transmitted Count RC
#define         RXDMTC		0x4120       //Rx Descriptor Minimum Threshold Count RC
#define         HGORCL		0x4128       //Host Good Octets Received Count (Lo) RC
#define         HGORCH		0x412C       //Host Good Octets Received Count (Hi) RC
#define         HGOTCL		0x4130       //Host Good Octets Transmitted Count (Lo) RC
#define         HGOTCH		0x4134       //Host Good Octets Transmitted Count (Hi) RC
#define         LENERRS		0x4138       //Length Errors Count Register RC
#define         SCVPC		0x4228       //SerDes/SGMII/1000BASE-KX Code Violation Packet Count Register RW
#define         MNGFBDPC	0x4154       //Management Full Buffer Drop Packet Count RC/W

         /* Manageability Statistics */
#define         BMNGPRC		0x413C       //MC Management Packets Receive Count RC
#define         BMRPDC		0x4140       //MC Management Receive Packets Dropped Count RC
#define         BMTPDC		0x8FDC       //MC Management Transmit Packets Dropped Count RC
#define         BMNGPTC		0x4144       //MC Management Packets Transmitted Count RC
#define         BUPRC		0x4400       //MC Total Unicast Packets Received RC
#define         BMPRC		0x4404       //MC Total Multicast Packets Received RC
#define         BBPRC		0x4408       //MC Total Broadcast Packets Received RC
#define         BUPTC		0x440C       //MC Total Unicast Packets Transmitted RC
#define         BMPTC		0x4410       //MC Total Multicast Packets Transmitted RC
#define         BBPTC		0x4414       //MC Total Broadcast Packets Transmitted RC
#define         BCRCERRS	0x4418       //MC FCS Receive Errors RC
#define         BALGNERRC	0x441C       //MC Alignment Errors RC
#define         BXONRXC		0x4420       //MC Pause XON Frames Received RC
#define         BXOFFRXC	0x4424       //MC Pause XOFF Frames Received RC
#define         BXONTXC		0x4428       //MC Pause XON Frames Transmitted RC
#define         BXOFFTXC	0x442C       //MC Pause XOFF Frames Transmitted RC
#define         BSCC		0x4430       //MC Single Collision Transmit Frames RC
#define         BMCC		0x4434       //MC Multiple Collision Transmit Frames RC

        /* Wake Up and Proxying */
#define         WUC		0x5800               //Wake Up Control RW
#define         WUFC		0x5808               //Wake Up Filter Control RW
#define         WUS		0x5810               //Wake Up Status R/W1C
#define         PROXYFC		0x5F60               //Proxying Filter Control RW
#define         PROXYS		0x5F64               //Proxying Status R/W1C
#define         IPAV		0x5838               //IP Address Valid RW
#define         IP4AT(n)	(0x5840 + (4 * n))   //IPv4 Address Table RW
#define         IP6AT(n)	(0x5880 + (4 * n))   //IPv6 Address Table RW
#define         WUPL		0x5900               //Wake Up Packet Length RO
#define         WUPM(n)		(0x5A00 + (4 * n))   //Wake Up Packet Memory RO
#define         FHFT(n)		(0x9000 + (4 * n))   //Flexible Host Filter Table Registers RW
#define         FHFT_EXT(n)	(0x9A00 + (4 * n))   //Flexible Host Filter Table Registers Extended RW
#define         PROXYFCEX	0x5590               //Proxy Filter Control Extended RW
#define         PROXYEXS	0x5594               //Proxy Extended Status R/W1C
#define         WFUTPF(n)	(0x5500 + (4 * n))   //[31:0] Wake Flex UDP TCP Port Filter RW
#define         RFUTPF		0x5580               //Range Flex UDP TCP Port Filter RW
#define         RWPFC		0x5584               //Range Wake Port Filter Control RW
#define         WFUTPS		0x5588               //Wake Filter UDP TCP Status R/W1C
#define         WCS		0x558C               //Wake Control Status R/W1C

         /* Manageability */
#define         MAVTV_7_0(n)		(0x5010 + (4 * (7 - n)))	//VLAN TAG Value 7 - 0 RW
#define         MFUTP_7_0(n)		(0x5030	+ (4 * (7 - n)))	//Management Flex UDP/TCP Ports [0-7] RW
#define         MFUTP_15_0(n)		(0x5070	+ (4 * (15 - n)))	//Management Flex UDP/TCP Ports [8-15] RW
#define         METF_3_0(n)		(0x5060	+ (4 * (3 - n)))	//Management Ethernet Type Filters RW
#define         MANC			0x5820             //Management Control RW
#define         MNGONLY			0x5864             //Management Only Traffic Register RW
#define         MSFM			0x5870             //Special Filters Modifiers RW
#define         MDEF_7_0(n)		(0x5890  + (4 * (7 - n)))	//Manageability Decision Filters RW
#define         MDEF_EXT_7_0(n)		(0x5930  + (4 * (7 - n)))	//Manageability Decision Filters RW
#define         MIPAF_15_0(n)		(0x58B0  + (4 * (15 - n)))	//Manageability IP Address Filter RW
#define         MMAL_3_0(n)		(0x5910 + 8*n)       //Manageability MAC Address Low 3:0 RW
#define         MMAH_3_0(n)		(0x5914 + 8*n)       //Manageability MAC Address High 3:0 RW
#define         FTFT(n)			(0x9400 + (4 * n))   //Flexible TCO Filter Table RW
#define         HOST_INT_MEM(n)		(0x8800 + (4 * n))   //Host Interface Memory Address Space RW
#define         HICR			0x8F00             //HOST Interface Control Register RW
#define         HIBBA			0x8F40             //Host Interface Buffer Base Address RW
#define         HIBMAXOFF		0x8F44             //Host Interface Buffer Maximum Offset RO

         /* PCIe */
#define         GCR		0x5B00       //PCIe Control Register RW
#define         GSCL_1		0x5B10       //PCIe Statistics Control #1 RW
#define         GSCL_2		0x5B14       //PCIe Statistics Control #2 RW
#define         GSCL_5_8(n)	(0x5B90 + (4 * (n - 5))) //PCIe Statistics Control Leaky Bucket Timer RW
#define         GSCN_0		0x5B20       //PCIe Counter Register #0 RW
#define         GSCN_1		0x5B24       //PCIe Counter Register #1 RW
#define         GSCN_2		0x5B28       //PCIe Counter Register #2 RW
#define         GSCN_3		0x5B2C       //PCIe Counter Register #3 RW
#define         FACTPS		0x5B30       //Function Active and Power State RW
#define         MREVID		0x5B64       //Mirrored Revision ID RO
#define         GCR_EXT		0x5B6C       //PCIe Control Extended Register RW
#define         DCA_CTRL	0x5B74       //DCA Control Register RW
#define         PICAUSE		0x5B88       //PCIe Interrupt Cause R/W1C
#define         PIENA		0x5B8C       //PCIe Interrupt Enable RW
#define         BARCTRL		0x5BFC       //PCIe BAR Control RW
#define         RR2DCDELAY	0x5BF4       //Read Request To Data Completion Delay Register RC
#define         PCIEMCTP	0x5B4C       //PCIe MCTP Register RW to FW

         /* Memory Error Detection */
#define         PEIND		0x1084       //Parity and ECC Indication RC
#define         PEINDM		0x1088       //Parity and ECC Indication Mask RW
#define         PBECCSTS	0x245C       //Packet Buffer ECC Status RW
#define         PCIEERRCTL	0x5BA0       //PCIe Parity Control Register RW
#define         PCIEECCCTL	0x5BA4       //PCIe ECC Control Register RW
#define         PCIEERRSTS	0x5BA8       //PCIe Parity Status Register R/W1C
#define         PCIEECCSTS	0x5BAC       //PCIe ECC Status Register R/W1C
#define         PCIEACL01	0x5B7C       //PCIe ACL0 and ACL1 Register RW to FW
#define         PCIEACL23	0x5B80       //PCIe ACL2 and ACL3 Register RW to FW
#define         LANPERRCTL	0x5F54       //LAN Port Parity Error Control Register RW to FW
#define         LANPERRSTS	0x5F58       //LAN Port Parity Error Status Register R/W1C
#define         LANPERRINJ	0x5F5C       //LAN Port Parity Error Inject Register SC

         /* Power Management Registers */
#define         DMACR		0x2508       //DMA Coalescing Control Register RW
#define         DMCTLX		0x2514       //DMA Coalescing Time to LX Request RW
#define         DMCTXTH		0x3550       //DMA Coalescing Transmit Threshold RW
#define         DMCCNT		0x5DD4       //DMA Coalescing Current Rx Count RO
#define         FCRTC		0x2170       //Flow Control Receive Threshold Coalescing RW
#define         DOBFFCTL	0x3F24       //DMA OBFF Control RW
#define         DMACTC		0x5DC8       //DMA Coalescing Clock Control Time Counter RO
#define         LTRMINV		0x5BB0       //Latency Tolerance Reporting (LTR) Minimum Values RW
#define         LTRMAXV		0x5BB4       //Latency Tolerance Reporting (LTR) Maximum Values RW
#define         LTRC		0x01A0       //Latency Tolerance Reporting (LTR) Control RW
#define         EEER		0x0E30       //Energy Efficient Ethernet (EEE) Register RW

         /* Diagnostic */
#define         PCIEMISC	0x5BB8  //PCIe Misc. Register RW

         /* PCS */
#define         PCS_CFG			0x4200       //PCS Configuration 0 Register RW
#define         PCS_LCTL		0x4208       //PCS Link Control Register RW
#define         PCS_LSTS		0x420C       //PCS Link Status Register RO
#define         PCS_DBG0		0x4210       //PCS Debug 0 Register RO
#define         PCS_DBG1		0x4214       //PCS Debug 1 Register RO
#define         PCS_ANADV		0x4218       //AN Advertisement Register RW
#define         PCS_LPAB		0x421C       //Link Partner Ability Register RO
#define         PCS_NPTX		0x4220       //AN Next Page Transmit Register RW
#define         PCS_LPABNP		0x4224       //Link Partner Ability Next Page Register RO

         /* Time Sync */
#define         TSYNCRXCTL		0xB620       //Rx Time Sync Control Register RW
#define         RXSTMPL			0xB624       //Rx Timestamp Low RO
#define         RXSTMPH			0xB628       //Rx Timestamp High RO
#define         TSYNCTXCTL		0xB614       //Tx Time Sync Control Register RW
#define         TXSTMPL			0xB618       //Tx Timestamp Value Low RO
#define         TXSTMPH			0xB61C       //Tx Timestamp Value High RO
#define         SYSTIMR			0xB6F8       //System Time Residue Register RW
#define         SYSTIML			0xB600       //System Time Register Low RW
#define         SYSTIMH			0xB604       //System Time Register High RW
#define         SYSTIMTM		0xB6FC       //System Time Register Tx MS RW
#define         TIMINCA			0xB608       //Increment Attributes Register RW
#define         TIMADJ			0xB60C       //Time Adjustment Offset Register RW
#define         TSAUXC			0xB640       //Auxiliary Control Register RW
#define         TRGTTIML0		0xB644       //Target Time Register 0 Low RW
#define         TRGTTIMH0		0xB648       //Target Time Register 0 High RW
#define         TRGTTIML1		0xB64C       //Target Time Register 1 Low RW
#define         TRGTTIMH1		0xB650       //Target Time Register 1 High RW
#define         FREQOUT0		0xB654       //Frequency Out 0 Control Register RW
#define         FREQOUT1		0xB658       //Frequency Out 1 Control Register RW
#define         AUXSTMPL0		0xB65C       //Auxiliary Timestamp 0 Register Low RO
#define         AUXSTMPH0		0xB660       //Auxiliary Timestamp 0 Register High RO
#define         AUXSTMPL1		0xB664       //Auxiliary Timestamp 1 Register Low RO
#define         AUXSTMPH1		0xB668       //Auxiliary Timestamp 1 Register High RO
#define         TSYNCRXCFG		0x5F50       //Time Sync Rx Configuration RW
#define         TSSDP			0x003C       //Time Sync SDP Configuration Register RW
#define         TSICR			0xB66C       //Time Sync Interrupt Cause Register RC/W1C
#define         TSIM			0xB674       //Time Sync Interrupt Mask Register RW
#define         LAUNCH_OS0		0x3578       //Launch Time Offset Register 0 RW

#define i210_writel(data, addr)	writel(data, (pdata->hw_addr+addr))
#define i210_readl(addr) readl(pdata->hw_addr + addr)
#endif
