/* i8255x.h - information for Intel Hub 10D/82567LM NIC */

#ifndef INTEL_VENDOR_ID
        #define INTEL_VENDOR_ID		0x8086
#endif
#define INTEL_82559ER_DEVICE_ID	0x1229

/* PCI Configuration Registers */

#define I8255X_PCI_COMMAND 	0x04
#define I8255X_PCI_STATUS 	0x06
#define I8255X_PCI_MEMBASE 	0x10
#define I8255X_PCI_IOBASE 	0x14
#define I8255X_PCI_FLASHBASE 	0x18
#define I8255X_PCI_IRQ 		0x3C

//FIXME : What are the following two entries for ?
#define I8255X_IO_IOADDR	0x00
#define I8255X_IO_IODATA	0x04

//#define I8255X_PCI_CMD_MASK	0x107

#define I8255X_RX_RING_SIZE 	32
#define I8255X_TX_RING_SIZE 	32
#define I8255X_RING_BOUNDARY 	16

/* Receive Descriptor */

struct i8255x_rx_desc {
	uint64  buffer_addr;		/* Address of the descriptor's 	*/
					/*  data buffer 		*/
	uint16  length;			/* Length of data DMAed into 	*/
					/*  data buffer 		*/
	uint16  csum;		   	/* Packet checksum 		*/
	uint8   status;		 	/* Descriptor status 		*/
	uint8   errors;		 	/* Descriptor Errors 		*/
	uint16  special;
};

/*
 * Check out i8255x_open.c for the specific bit structure of 
 * the command and status words of both _tx_desc and _rx_desc
 */
struct	i82559_tx_desc {
	unsigned short status;
	unsigned short command;
	uint32 link_addr;
	uint32 tbd_addr;
	unsigned short byte_count;
	byte tx_treshold;
	byte tbd_no;
};

struct	i82559_rx_desc {
	unsigned short status;
	unsigned short command;
	uint32 link_addr;
	uint32 reserved;
	unsigned short ac_count;
	unsigned short size;
};

/* Transmit Descriptor */

struct i8255x_tx_desc {
	uint64  buffer_addr;		/* Address of the descriptor's  */
					/*  data buffer 		*/
	union {
	 uint32 data;
	 struct {
	  uint16 	length; 	/* Data buffer length 		*/
	  uint8 	cso;		/* Checksum offset 		*/
	  uint8 	cmd;		/* Descriptor control 		*/
	 } flags;
	} lower;
	union {
	 uint32 data;
	 struct {
	  uint8 	status; 	/* Descriptor status 		*/
	  uint8 	css;		/* Checksum start 		*/
	  uint16 	special;
	 } fields;
	} upper;
};

#define I8255X_RDSIZE 		sizeof(struct 	i8255x_rx_desc)
#define I8255X_TDSIZE 		sizeof(struct 	i8255x_tx_desc)


#define I82559_RFDSIZE 	sizeof(struct 	i82559_rx_desc)
#define I82559_TFDSIZE 	sizeof(struct 	i82559_tx_desc)

static 	inline 	void 	_i8255x_io_writel(
	uint32 	iobase,
	uint32 	reg,
	uint32 	val
	)
{
	outl(iobase + I8255X_IO_IOADDR, reg);
	outl(iobase + I8255X_IO_IODATA, val);
}

static 	inline 	uint32 	_i8255x_io_readl(
	uint32 	iobase,
	uint32 	reg
	)
{
	outl(iobase + I8255X_IO_IOADDR, reg);
	return inl(iobase + I8255X_IO_IODATA);
}

#define i8255x_io_writel(reg, val) 					\
	_i8255x_io_writel(ethptr->iobase, I8255X_##reg, (val))
#define i8255x_io_readl(reg) 						\
	_i8255x_io_readl(ethptr->iobase, I8255X_##reg)
#define i8255x_io_flush()	 					\
	_i8255x_io_readl(ethptr->iobase, I8255X_STATUS);

#ifndef ADDR_BIT_MASK
        #define ADDR_BIT_MASK   		0xFFFFFF	
#endif
#define I8255X_ICH_FWSM_RSPCIPHY 	0x00000040 
					/* Reset PHY on PCI Reset */
#define I8255X_ICH_LAN_INIT_TIMEOUT 	1500
#define I8255X_ICH_RAR_ENTRIES 		7
#define SW_FLAG_TIMEOUT 		1000 	
					/* SW Semaphore flag timeout in */
					/* 	milliseconds 		*/

/* Extended Device Control */

#define I8255X_CTRL_EXT_RO_DIS 	0x00020000 	/*Relaxed Ordering disable*/

/* Receive Descriptor bit definitions */

#define I8255X_RXD_STAT_DD 	0x01 		/* Descriptor Done 	*/

/* Receive Control */

#define I8255X_RCTL_EN 		0x00000002 	/* enable 		*/
#define I8255X_RCTL_SBP 	0x00000004 	/* store bad packet 	*/
#define I8255X_RCTL_LPE 	0x00000020 	/* long packet enable 	*/
#define I8255X_RCTL_LBM_NO 	0x00000000 	/* no loopback mode 	*/
#define I8255X_RCTL_DTYP_MASK 	0x00000C00 	/* Descriptor type mask */
#define I8255X_RCTL_RDMTS_HALF 	0x00000000 	/*rx desc min thresh size*/
#define I8255X_RCTL_MO_SHIFT 	12 		/*multicast offset shift*/
#define I8255X_RCTL_BAM 	0x00008000 	/* broadcast enable 	*/

/* these buffer sizes are valid if I8255X_RCTL_BSEX is 0 */

#define I8255X_RCTL_SZ_2048 	0x00000000 	/* rx buffer size 2048 */

/* these buffer sizes are valid if I8255X_RCTL_BSEX is 1 */

#define I8255X_RCTL_SZ_4096 	0x00030000 	/* rx buffer size 4096 	*/
#define I8255X_RCTL_PMCF 	0x00800000 	/*pass MAC control frames*/
#define I8255X_RCTL_BSEX 	0x02000000 	/* Buffer size extension */
#define I8255X_RCTL_SECRC 	0x04000000 	/* Strip Ethernet CRC 	*/
#define I8255X_RCTL_FLXBUF_MASK 0x78000000 	/* Flexible buffer size */

/* Device Control */

#define I8255X_CTRL_GIO_MASTER_DISABLE 0x00000004
					/*Blocks new Master reqs 	*/
#define I8255X_CTRL_SLU 	0x00000040	
					/* Set link up (Force Link) 	*/
#define I8255X_CTRL_FRCSPD 	0x00000800 	/* Force Speed 		*/
#define I8255X_CTRL_FRCDPX 	0x00001000 	/* Force Duplex 	*/
#define I8255X_CTRL_RST 	0x04000000 	/* Global reset 	*/
#define I8255X_CTRL_RFCE 	0x08000000 	
					/* Receive Flow Control enable 	*/
#define I8255X_CTRL_TFCE 	0x10000000 	
					/* Transmit flow control enable */
#define I8255X_CTRL_PHY_RST 	0x80000000 	/* PHY Reset 		*/

/* Device Status */

#define I8255X_STATUS_LAN_INIT_DONE 0x00000200  
					/* Lan Init Completion by NVM 	*/
#define I8255X_STATUS_PHYRA 	0x00000400      
					/* PHY Reset Asserted 		*/
#define I8255X_STATUS_GIO_MASTER_ENABLE 0x00080000 
					/* Master request status 	*/

/* Transmit Descriptor bit definitions */

#define I8255X_TXD_CMD_EOP 	0x01000000 	/* End of Packet 	*/
#define I8255X_TXD_CMD_IFCS 	0x02000000 	
					/* Insert FCS (Ethernet CRC) 	*/
#define I8255X_TXD_CMD_IC 	0x04000000 	/* Insert Checksum 	*/
#define I8255X_TXD_CMD_RS 	0x08000000 	/* Report Status 	*/
#define I8255X_TXD_CMD_DEXT 	0x20000000 
					/* Descriptor extension 	*/
#define I8255X_TXD_CMD_IDE 	0x80000000 	/* Enable Tidv register */
#define I8255X_TXD_STAT_DD 	0x00000001 	/* Descriptor Done 	*/

/* Transmit Control */

#define I8255X_TCTL_EN 		0x00000002 	/* enable tx 		*/
#define I8255X_TCTL_PSP 	0x00000008 	/* pad short packets 	*/
#define I8255X_TCTL_COLD 	0x003ff000 	/* collision distance 	*/
#define I8255X_TCTL_CT 		0x00000ff0 	/* collision threshold 	*/
#define I8255X_TCTL_RTLC 	0x01000000 	
					/* Re-transmit on late collision*/

/* Receive Checksum Control */

#define I8255X_RXCSUM_TUOFL 	0x00000200   	
					/* TCP/UDP checksum offload 	*/

/* Header split receive */

#define I8255X_RFCTL_NFSW_DIS 	0x00000040
#define I8255X_RFCTL_NFSR_DIS 	0x00000080

/* Extended Configuration Control and Size */

#define I8255X_EXTCNF_CTRL_SWFLAG 0x00000020
#define I8255X_KABGTXD_BGSQLBIAS 0x00050000

/* Interrupt Cause Read */

#define I8255X_ICR_TXDW 	0x00000001 
					/* Transmit desc written back 	*/
#define I8255X_ICR_LSC  	0x00000004 	/* Link Status Change 	*/
#define I8255X_ICR_RXSEQ 	0x00000008 	/* rx sequence error 	*/
#define I8255X_ICR_RXO 		0x00000040 	/* rx overrun 		*/
#define I8255X_ICR_RXT0 	0x00000080 	/*rx timer intr (ring 0)*/

/*
 * This defines the bits that are set in the Interrupt Mask
 * Set/Read Register.  Each bit is documented below:
 * 	o RXT0   = Receiver Timer Interrupt (ring 0)
 * 	o TXDW   = Transmit Descriptor Written Back
 * 	o RXSEQ  = Receive Sequence Error
 * 	o LSC    = Link Status Change
 */
//FIXME : This needs to be CHANGED
#ifndef IMS_ENABLE_MASK
#define IMS_ENABLE_MASK ( 						\
				I8255X_IMS_TXDW 		|	\
				I8255X_IMS_LSC 			|	\
				I8255X_IMS_RXSEQ 		|	\
				I8255X_IMS_RXT0 		|	\
				I8255X_IMS_RXO)
#endif

/* Interrupt Mask Set */

#define I8255X_IMS_TXDW I8255X_ICR_TXDW 	/* Tx desc written back */
#define I8255X_IMS_LSC 	I8255X_ICR_LSC 		/* Link Status Change 	*/
#define I8255X_IMS_RXSEQ I8255X_ICR_RXSEQ 	/* rx sequence error 	*/
#define I8255X_IMS_RXO  I8255X_ICR_RXO 		/* rx overrun 		*/
#define I8255X_IMS_RXT0 I8255X_ICR_RXT0 	/* rx timer intr 	*/

/* Interrupt Cause Set */

#define I8255X_ICS_TXDW I8255X_ICR_TXDW 	/* Tx desc written back */
#define I8255X_ICS_LSC 	I8255X_ICR_LSC 		/* Link Status Change 	*/
#define I8255X_ICS_RXO 	I8255X_ICR_RXO 		/* rx overrun 		*/
#define I8255X_ICS_RXT0 I8255X_ICR_RXT0 	/* rx timer intr 	*/

/* Transmit Descriptor Control */

#define I8255X_TXDCTL_PTHRESH 	0x0000003F 	
					/* TXDCTL Prefetch Threshold 	*/
#define I8255X_TXDCTL_WTHRESH 	0x003F0000 	
					/* TXDCTL Writeback Threshold 	*/
#define I8255X_TXDCTL_GRAN    	0x01000000 	/* TXDCTL Granularity 	*/
#define I8255X_TXDCTL_FULL_TX_DESC_WB 0x01010000 /* GRAN=1, WTHRESH=1 	*/
#define I8255X_TXDCTL_MAX_TX_DESC_PREFETCH 0x0100001F 
					/* GRAN=1, PTHRESH=31 		*/

/* Receive Address */
/*
 * Number of high/low register pairs in the RAR. The RAR (Receive Address
 * Registers) holds the directed and multicast addresses that we monitor.
 * Technically, we have 16 spots.  However, we reserve one of these spots
 * (RAR[15]) for our directed address used by controllers with
 * manageability enabled, allowing us room for 15 multicast addresses.
 */
#define I8255X_RAL_MAC_ADDR_LEN 	4
#define I8255X_RAH_MAC_ADDR_LEN 	2

/* Collision related configuration parameters */

#define I8255X_HDX_COLLISION_DISTANCE 	511
#define I8255X_COLD_SHIFT 		12

/* Default values for the transmit IPG register */

/*
#define DEFAULT_82543_TIPG_IPGT_FIBER 	9
#define DEFAULT_82543_TIPG_IPGT_COPPER 	8

#define DEFAULT_82543_TIPG_IPGR1 	8
#define I8255X_TIPG_IPGR1_SHIFT 	10

#define DEFAULT_82543_TIPG_IPGR2 	6
#define DEFAULT_80003ES2LAN_TIPG_IPGR2 	7
#define I8255X_TIPG_IPGR2_SHIFT 	20
*/

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
/*
#define REQ_TX_DESCRIPTOR_MULTIPLE 	8
#define REQ_RX_DESCRIPTOR_MULTIPLE 	8
*/

/* Number of 100 microseconds we wait for PCI Express master disable */
/*
#define MASTER_DISABLE_TIMEOUT 		800
*/
/* Number of milliseconds we wait for PHY configuration done after */
/* 	MAC reset */
/*
#define PHY_CFG_TIMEOUT 		100
*/

/* PCI Express Control */
/*
#define I8255X_GCR_RXD_NO_SNOOP 	0x00000001
#define I8255X_GCR_RXDSCW_NO_SNOOP 	0x00000002
#define I8255X_GCR_RXDSCR_NO_SNOOP 	0x00000004
#define I8255X_GCR_TXD_NO_SNOOP 	0x00000008
#define I8255X_GCR_TXDSCW_NO_SNOOP 	0x00000010
#define I8255X_GCR_TXDSCR_NO_SNOOP 	0x00000020

#define PCIE_NO_SNOOP_ALL ( 						\
				I8255X_GCR_RXD_NO_SNOOP 	|	\
				I8255X_GCR_RXDSCW_NO_SNOOP 	| 	\
				I8255X_GCR_RXDSCR_NO_SNOOP 	| 	\
				I8255X_GCR_TXD_NO_SNOOP 	| 	\
				I8255X_GCR_TXDSCW_NO_SNOOP 	| 	\
				I8255X_GCR_TXDSCR_NO_SNOOP)
*/
/* PHY Control Register */
#define MII_CR_RESTART_AUTO_NEG 0x0200  /* Restart auto negotiation 	*/
#define MII_CR_AUTO_NEG_EN 	0x1000  /* Auto Neg Enable 		*/
#define MII_CR_RESET 		0x8000  /* 0 = normal, 1 = PHY reset 	*/

/* PHY Status Register */
#define MII_SR_LINK_STATUS 	0x0004 	/* Link Status 1 = link 	*/
#define MII_SR_AUTONEG_COMPLETE 0x0020 	/* Auto Neg Complete 		*/

/* Autoneg Advertisement Register */

#define NWAY_AR_10T_HD_CAPS 	0x0020 	/* 10T   Half Duplex Capable 	*/
#define NWAY_AR_10T_FD_CAPS 	0x0040 	/* 10T   Full Duplex Capable 	*/
#define NWAY_AR_100TX_HD_CAPS	0x0080 	/* 100TX Half Duplex Capable 	*/
#define NWAY_AR_100TX_FD_CAPS	0x0100  /* 100TX Full Duplex Capable 	*/
#define NWAY_AR_PAUSE 		0x0400 	/* Pause operation desired 	*/
#define NWAY_AR_ASM_DIR 	0x0800 	/*Asymmetric Pause Direction bit*/

/* 1000BASE-T Control Register */

#define CR_1000T_HD_CAPS 	0x0100 	/* Advertise 1000T HD capability*/
#define CR_1000T_FD_CAPS 	0x0200 	/* Advertise 1000T FD capability*/

/* PHY Registers defined by IEEE */

#define PHY_CONTROL 		0x00 	/* Control Register 		*/
#define PHY_STATUS 		0x01 	/* Status Register 		*/
#define PHY_AUTONEG_ADV 	0x04 	/* Autoneg Advertisement 	*/
#define PHY_1000T_CTRL 		0x09 	/* 1000Base-T Control Reg 	*/
#define PHY_PAGE_SELECT 	0x15 	/* Page Address Reg 		*/

#define PHY_PAGE_SHIFT 		5
#define BM_PHY_PAGE_SELECT 	22 	/* Page Select for BM 		*/
#define PHY_CONTROL_LB 		0x4000 	/* PHY Loopback bit 		*/

#ifndef ETH_ALEN
#define ETH_ALEN 		6
#endif

#define PHY_REVISION_MASK 	0xFFFFFFF0
#define MAX_PHY_REG_ADDRESS 	0x1F 	/* 5 bit address bus (0-0x1F) 	*/
#define MAX_PHY_MULTI_PAGE_REG 	0xF

/* M88E1000 Specific Registers */

#define M88I8255X_PHY_SPEC_CTRL 0x10 	/* PHY Specific Control Register*/

/* M88E1000 PHY Specific Control Register */

#define M88I8255X_PSCR_POLARITY_REVERSAL 0x0002 
					/* 1=Polarity Reverse enabled 	*/
/* Auto crossover enabled all speeds */
#define M88I8255X_PSCR_AUTO_X_MODE 	0x0060

/* BME1000 PHY Specific Control Register */
#define BMI8255X_PSCR_ENABLE_DOWNSHIFT 	0x0800 /* 1 = enable downshift */

/* MDI Control */

#define I8255X_MDIC_REG_SHIFT 	16
#define I8255X_MDIC_PHY_ADDR  	2
#define I8255X_MDIC_PHY_SHIFT 	21
#define I8255X_MDIC_OP_WRITE  	0x04000000
#define I8255X_MDIC_OP_READ 	0x08000000
#define I8255X_MDIC_READY 	0x10000000
#define I8255X_MDIC_ERROR 	0x40000000

/* SerDes Control */

#define I8255X_GEN_POLL_TIMEOUT 	640

#define I8255X_KMRNCTRLSTA_OFFSET 	0x001F0000
#define I8255X_KMRNCTRLSTA_OFFSET_SHIFT 16
#define I8255X_KMRNCTRLSTA_REN          0x00200000

#define I8255X_KMRNCTRLSTA_TIMEOUTS 	0x4 	/* Kumeran Timeouts 	*/
#define I8255X_KMRNCTRLSTA_INBAND_PARAM 0x9 	
					/* Kumeran InBand Parameters 	*/

#define I8255X_MTA_NUM_ENTRIES 	32

#define I8255X_PBA_DEFAULT 	0x000E000A

#define I8255X_TIDV_DEFAULT 	8       /* Transmit Interrupt Delay 	*/
#define I8255X_TADV_DEFAULT 	32 	/* Transmit Absolute Interrupt 	*/
					/* 	Delay 			*/
#define I8255X_RDTR_DEFAULT 	0       /* Receive Interrupt Delay 	*/
#define I8255X_RADV_DEFAULT 	8       /* Receive Absolute Interrupt 	*/
					/* 	Delay 			*/
#define I8255X_ITR_DEFAULT 	20000   /* Interrupt Throttling Rate 	*/

/* Collision related configuration parameters */

#define I8255X_COLLISION_THRESHOLD 	15
#define I8255X_CT_SHIFT 		4
#define I8255X_COLLISION_DISTANCE 	63

#define I8255X_EEPROM_REG		0x0e
#define I8255X_EESK		0x01
#define I8255X_EECS		0x02
#define I8255X_EEDI		0x04
#define I8255X_EEDO		0x08

#define I8255X_SCB_STATUS	0x00


#define I8255X_STATUS_LOW	0x00
#define I8255X_STATUS_HIGH	0x01
#define I8255X_SCB_COMMAND_LOW	0x02
#define I8255X_SCB_COMMAND_HIGH	0x03
#define I8255X_SCB_GENPTR	0x04
#define I8255X_SCB_PORT		0x08

#define I8255X_SCB_SHIFT_CUC	4
#define I8255X_SCB_SHIFT_RUC	0

#define I8255X_STATUS_CU_MASK	0xC0
#define I8255X_STATUS_RU_MASK	0x3C

#define I8255X_COMMAND_CU_START 0x10

#define I8255X_STATUS_CU_IDLE	0x00
#define I8255X_STATUS_CU_SUSP	0x40
#define I8255X_STATUS_CU_LPQA	0x80
#define I8255X_STATUS_CU_HQPA	0xC0

#define I8255X_COMMAND_RU_START 0x01
#define I8255X_COMMAND_RU_RESUME 0x02

#define I8255X_STATUS_RU_IDLE	0x00
#define I8255X_STATUS_RU_SUSP	0x04
#define I8255X_STATUS_RU_NORES	0x08
#define I8255X_STATUS_RU_READY	0x10

#define I8255X_CMD_ACCEPT_RETRIES 20

#define I8255X_TX_STATUS_C 	0x8000
#define I8255X_TX_STATUS_OK 	0x2000
#define I8255X_TX_STATUS_U 	0x1000

#define I8255X_RX_STATUS_C 	0x8000
#define I8255X_RX_STATUS_OK 	0x2000

/*Interrupt masks for command word*/
/*Interrupt when a action command is complete*/
#define I8255X_INTR_CX_MASK	0x8000
/*Interrupt when a RU recieves a frame*/
#define I8255X_INTR_FR_MASK	0x4000
/*Interrupt when CU leaves the active state*/
#define I8255X_INTR_CNA_MASK	0x2000
#define I8255X_INTR_RNX_MASK	0x1000
/*Interrupt when a RU recieves a predefined part of frame*/
#define I8255X_INTR_ER_MASK	0x0800
/*Interrupt when a Flow Control frame is recieved*/
#define I8255X_INTR_FCP_MASK	0x0400


#define I8255X_INTR_SI_MASK	0x0200
#define I8255X_INTR_M_MASK	0x0100 				/*higher precedence over all the above*/

/*All the interrupts in this mask will be enabled. 
 * SI and M should not be used in this mask */
#define I8255X_INTR_ENABLE_MASK	(I8255X_INTR_CX_MASK|I8255X_INTR_FR_MASK|I8255X_INTR_CNA_MASK)

//FIXME may be we can use same flags by shifting appropriately
/*Interrupt masks for status word*/

/*Interrupt when a action command is complete*/
#define I8255X_INTR_CX_MASK_STATUS	0x0080
/*Interrupt when a RU recieves a frame*/
#define I8255X_INTR_FR_MASK_STATUS	0x0040
/*Interrupt when CU leaves the active state*/
#define I8255X_INTR_CNA_MASK_STATUS	0x0020

#define I8255X_INTR_ALL_MASK_STATUS	(I8255X_INTR_CX_MASK_STATUS | I8255X_INTR_FR_MASK_STATUS | I8255X_INTR_CNA_MASK_STATUS )

#define I82559_DUMP_STATS_SIZE	84	/*84 bytes are necessary for the stats dump commands on i82559 and should be D-word alligned*/
