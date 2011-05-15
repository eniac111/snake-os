#include <common.h>

#ifdef CONFIG_STAR_NIC

#include <malloc.h>
#include <net.h>
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_nic.h>
#include "star_nic.h"

//#define STAR_NIC_DEBUG

#define WAIT_FOR_LINK		3 // wait seconds before net transmitting when link is up

#define CPU_CACHE_BYTES		16
#define CPU_CACHE_ALIGN(X)	(((X) + (CPU_CACHE_BYTES-1)) & ~(CPU_CACHE_BYTES-1))

static struct star_nic_dev_t star_nic_dev;
static u8 star_nic_dev_initialized;

static u8 default_mac_addr[] = {0x08, 0xaa, 0xbb, 0xcc, 0xdd, 0xee};

typedef struct
{   
	u32 vid;	//0~4095
	u32 control;	//ENABLE or DISABLE
} my_vlan_entry_t;

static my_vlan_entry_t my_vlan_id[4] =
{
	{ 2, 0},	//value for my_vid0
	{ 2, 1},	//value for my_vid1
	{ 1, 1},	//value for my_vid2
	{ 1, 0}		//value for my_vid3
};

static TX_DESC_T	*tx_desc_pool;
static RX_DESC_T	*rx_desc_pool;
static pkt_t		*pkt_pool;
static u8		*pkt_buffer_pool;
static u8		mem_alloc_ok;

//=============================================================================
#ifdef CONFIG_STAR_NIC_PHY_INTERNAL

static int star_nic_write_phy(u8 phy_addr, u8 phy_reg, u16 write_data);
static int star_nic_read_phy(u8 phy_addr, u8 phy_reg, u16 *read_data);

// LED def for internal phy
#define FE_PHY_LED_MODE			(0x1 << 12)
#define INTERNAL_PHY_PATCH_CHECKCNT	16

static u16 long_cable_global_reg[32]={
0x0000,0x19a0,0x1d00,0x0e80,0x0f60,0x07c0,0x07e0,0x03e0,
0x0000,0x0000,0x0000,0x2000,0x8250,0x1700,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x204b,0x01c2,0x0000,
0x0000,0x0000,0x0fff,0x4100,0x9319,0x0021,0x0034,0x270a|FE_PHY_LED_MODE
};

static u16 long_cable_local_reg[32]={
0x3100,0x786d,0x01c1,0xca51,0x05e1,0x45e1,0x0003,0x001c,
0x2000,0x9828,0xf3c4,0x400c,0xf8ff,0x6940,0xb906,0x503c,
0x8000,0x297a,0x1010,0x5010,0x6ae1,0x7c73,0x783c,0xfbdf,
0x2080,0x3244,0x1301,0x1a80,0x8e8f,0x8000,0x9c29,0xa70a|FE_PHY_LED_MODE
};

/*=============================================================*
 *  eth3220ac_rt8101_phy_setting
 *=============================================================*/
static  void eth3220ac_rt8101_phy_setting(int port)
{
	star_nic_write_phy(port, 12, 0x18ff);
	star_nic_write_phy(port, 18, 0x6400);
}

static void eth3220ac_release_bpf(int port)
{
	star_nic_write_phy(port, 18, 0x6210);
}

static  void eth3220ac_def_bpf(int port)
{
	star_nic_write_phy(port, 18, 0x6bff);
}

static  void eth3220ac_def_linkdown_setting(int port)
{
	star_nic_write_phy(port, 13, 0xe901);
	star_nic_write_phy(port, 14, 0xa3c6);
}

static  void eth3220ac_def_linkup_setting(int port)
{
	star_nic_write_phy(port, 13, 0x6901);
	star_nic_write_phy(port, 14, 0xa286);
}

/*=============================================================*
 *  eth3220ac_unlink_agc:
 *=============================================================*/
static void eth3220ac_unlink_agc(int port)
{
	// start AGC adaptive
	star_nic_write_phy(port, 15, 0xa050);
}

/*=============================================================*
 *  eth3220ac_rt8100_check
 *=============================================================*/
static int eth3220ac_rt8100_check(int port)
{
	u16 reg, reg2;

	/* Read reg27 (error register) */
	star_nic_read_phy(port, 27, &reg);
	/* if error exists, set Bypass Filter enable */
	if ((reg & 0xfffc)) {
                star_nic_read_phy(port, 15, &reg);	
                star_nic_read_phy(port, 27, &reg2);	
		if (( reg2 & 0xfffc) && (((reg >> 9) & 0xff) < 0x1c)) {
			printf("8100 pos err\n");
			/* Bypass agcgain disable */
			star_nic_write_phy(port, 15, (reg & (~(0x1 << 7))));
			
			/* repeat counts when reaching threshold error */
			star_nic_write_phy(port, 13, 0x4940);
			
			/* Speed up AN speed && compensate threshold phase error */
			star_nic_write_phy(port, 14, 0xa306);
			
			/* Bypass Filter enable */
                        star_nic_read_phy(port, 18, &reg2);	

			star_nic_write_phy(port, 18, (reg | 0x400));
			
			/* restart AN */
			star_nic_write_phy(port, 0, 0x3300);
			return 1;
		}
	}
	return 0;
}


/*=============================================================*
 *  eth3220ac_rt8100_linkdown
 *=============================================================*/
static void eth3220ac_rt8100_linkdown(int port)
{
	u16 reg;
	
	/* Bypass Filter disable */
	star_nic_read_phy(port, 18, &reg);	
	star_nic_write_phy(port, 18, (reg & (~(0x1 << 10))));
	eth3220ac_def_linkdown_setting(port);
}

static  void eth3220ac_normal_phy_setting(int port)
{
	star_nic_write_phy(port, 12, 0xd8ff);
	eth3220ac_def_bpf(port);
}

/*=============================================================*
 *  eth3220_phyinit:
 *=============================================================*/
static void eth3220ac_10m_agc(void)
{
	/* Force 10M AGC = 2c globally */
	star_nic_write_phy(0, 31, 0x2f1a);
	star_nic_write_phy(0, 12, 0x112c);
	star_nic_write_phy(0, 13, 0x2e21);
	star_nic_write_phy(0, 31, 0xaf1a);
}

static void eth3220ac_dfe_init(void)
{
	int i;

	star_nic_write_phy(0, 31, 0x2f1a);
	for (i = 0; i <= 7; i++)
		star_nic_write_phy(0, i, 0);
	star_nic_write_phy(0, 11, 0x0b50);
	star_nic_write_phy(0, 31, 0xaf1a);
}

static void eth3220ac_phy_cdr_training_init(void)
{
	/* Force all port in 10M FD mode */
	star_nic_write_phy(0, 0, 0x100);
	
	/* Global setting */
	star_nic_write_phy(0, 31, 0x2f1a);
	star_nic_write_phy(0, 29, 0x5021);
        udelay(2000); //2ms, wait > 1 ms
	star_nic_write_phy(0, 29, 0x4021);
        udelay(2000); //2ms, wait > 1 ms
	star_nic_write_phy(0, 31, 0xaf1a);

	/* Enable phy AN */
	star_nic_write_phy(0, 0, 0x3100);	
}

static void eth3220_phyinit(void)
{
	eth3220ac_10m_agc();
	eth3220ac_dfe_init();
	eth3220ac_phy_cdr_training_init();
}

static void eth3220_phycfg(int phyaddr)
{
	eth3220ac_def_linkdown_setting(phyaddr);
	eth3220ac_normal_phy_setting(phyaddr);
	star_nic_write_phy(phyaddr, 9, 0x7f);
}

static void internal_phy_patch_check(void)
{
	u32 short_cable_agc_detect_count = 0;
	u32 link_status, link_speed;
	u32 ii, jj;
	u16 phy_data;
	u16 phy_data2;

	{
		int i;
		u32 last_time;

		printf("Check Link Status ");
		reset_timer_masked();
		last_time = get_timer_masked();
		while (1) {
			star_nic_read_phy(STAR_NIC_PHY_ADDR, 1, &phy_data);
			udelay(100);
			star_nic_read_phy(STAR_NIC_PHY_ADDR, 1, &phy_data2);
			udelay(100);
			if (((phy_data & 0x0004) != 0x0004) && ((phy_data2 & 0x0004) != 0x0004)) { // link down
				star_nic_read_phy(STAR_NIC_PHY_ADDR, 15, &phy_data);
				if (((phy_data) & 0x7F) <= 0x12) { // short cable
					short_cable_agc_detect_count++;
					break;
				}
			} else {
				break;
			}
			if ((get_timer_masked() - last_time) > (1 * CFG_HZ)) {
				last_time = get_timer_masked();
				printf(".");
			}
			if (get_timer_masked() > (3 * CFG_HZ)) {
				break;
			}
			for (i = 0; i < 100; i++) {
				udelay(1000);
			}
		}
	}

	if (short_cable_agc_detect_count) { // short cable
		eth3220_phyinit();
		eth3220_phycfg(0);
	} else { // long cable
		// set to global domain
		star_nic_write_phy(NIC_PHY_ADDRESS, 31, 0x2f1a);
		for (ii = 0; ii < 32; ii++) {
			star_nic_write_phy(NIC_PHY_ADDRESS, ii, long_cable_global_reg[ii]);
		}
		// set to local domain
		star_nic_write_phy(NIC_PHY_ADDRESS, 31, 0xaf1a);
		for (ii = 0; ii < 32; ii++) {
			star_nic_write_phy(NIC_PHY_ADDRESS, ii, long_cable_local_reg[ii]);
		}
	}
}
#endif // CONFIG_STAR_NIC_PHY_INTERNAL
//=============================================================================

#ifdef STAR_NIC_DEBUG
static u32 debug_last_time = 0xffffffff;
void print_nic_debug();

static void star_nic_show_format_reg(u32 val)
{
	int i;

	for (i = 31; i >= 0; i--) {
		if (val & (((unsigned long)0x1) << i)) {
			printf("[%02d:1] ", i);
		} else {
			printf("[%02d:0] ", i);
		}
		if ((i % 4) == 0) {
			printf("\n");
		}
	}
	printf("================================\n");
}

static void star_nic_show_reg(void)
{
	u32 reg_val;

	reg_val = NIC_MEM_MAP_VALUE(0x000);
	printf("NIC REG OFF 0x000: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x004);
	printf("NIC REG OFF 0x004: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x008);
	printf("NIC REG OFF 0x008: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x00C);
	printf("NIC REG OFF 0x00C: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x010);
	printf("NIC REG OFF 0x010: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x014);
	printf("NIC REG OFF 0x014: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x018);
	printf("NIC REG OFF 0x018: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x01C);
	printf("NIC REG OFF 0x01C: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x020);
	printf("NIC REG OFF 0x020: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x024);
	printf("NIC REG OFF 0x024: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x028);
	printf("NIC REG OFF 0x028: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x030);
	printf("NIC REG OFF 0x030: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x034);
	printf("NIC REG OFF 0x034: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x038);
	printf("NIC REG OFF 0x038: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x03C);
	printf("NIC REG OFF 0x03C: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x040);
	printf("NIC REG OFF 0x040: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x044);
	printf("NIC REG OFF 0x044: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x048);
	printf("NIC REG OFF 0x048: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x04C);
	printf("NIC REG OFF 0x04C: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x050);
	printf("NIC REG OFF 0x050: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x054);
	printf("NIC REG OFF 0x054: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x058);
	printf("NIC REG OFF 0x058: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);

	reg_val = NIC_MEM_MAP_VALUE(0x05C);
	printf("NIC REG OFF 0x05C: 0x%08x\n", reg_val);
	star_nic_show_format_reg(reg_val);
}
#endif

static int star_nic_write_phy(u8 phy_addr, u8 phy_reg, u16 write_data)
{
	int i;

	if (phy_addr > 31) {
		return 0;
	}

	//clear previous rw_ok status
	NIC_PHY_CONTROL_REG0 = (0x1 << 15);
 
	NIC_PHY_CONTROL_REG0 = ((phy_addr & 0x1F) |   
		((phy_reg & 0x1F) << 8) |
		(0x1 << 13) |
		((write_data & 0xFFFF) << 16));

	for (i = 0; i < 10000; i++) {
		// if write command completed
		if ((NIC_PHY_CONTROL_REG0) & (0x1 << 15)) {
			// clear the rw_ok status, and clear other bits value
			NIC_PHY_CONTROL_REG0 = (0x1 << 15);
			return (0);    /* for ok indication */
		}
		udelay(1000);
	}

	return (-1);    /* for failure indication */
}

static int star_nic_read_phy(u8 phy_addr, u8 phy_reg, u16 *read_data)
{
	u32 status;
	int i;

	if (phy_addr > 31) {
		return 0;
	}

	// clear previous rw_ok status
	NIC_PHY_CONTROL_REG0 = (0x1 << 15);

	NIC_PHY_CONTROL_REG0 = ((phy_addr & 0x1F) | 
		((phy_reg & 0x1F) << 8) | 
		(0x1 << 14));    

	for (i = 0; i < 10000; i++) {
		status = NIC_PHY_CONTROL_REG0;
		if (status & (0x1 << 15)) {
			// clear the rw_ok status, and clear other bits value
			NIC_PHY_CONTROL_REG0 = (0x1 << 15);
			*read_data = (u16)((status >> 16) & 0xFFFF);
			return (0);    /* for ok indication */
		}
		udelay(1000);
	}

	return (-1);    /* for failure indication */
}

static inline void star_nic_phy_powerdown(struct star_nic_dev_t *dev)
{
	u16 phy_data;
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	star_nic_read_phy(dev->phy_addr, 0, &phy_data);
	phy_data |= (0x1 << 11);
	star_nic_write_phy(dev->phy_addr, 0, phy_data);

	// set hight
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 15);
	// set low
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << 15);
}

static inline void star_nic_phy_powerup(struct star_nic_dev_t *dev)
{
	u16 phy_data;
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	star_nic_read_phy(dev->phy_addr, 0, &phy_data);
	phy_data &= ~(0x1 << 11);
	star_nic_write_phy(dev->phy_addr, 0, phy_data);

	// set hight
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 15);
}

static inline pkt_t *star_nic_alloc_pkt(struct star_nic_dev_t *dev)
{
	pkt_t *pkt;

	pkt = dev->free_pkt_list;
	if (pkt) {
		dev->free_pkt_list = pkt->next;
		pkt->next = NULL;
		dev->free_pkt_count--;
	}

	return pkt;
}

static inline void star_nic_free_pkt(struct star_nic_dev_t *dev, pkt_t *pkt)
{
	pkt->next = dev->free_pkt_list;
	dev->free_pkt_list = pkt;
	dev->free_pkt_count++;
}

static void star_nic_tx_pkt_enqueue(struct star_nic_dev_t *dev, pkt_t *pkt)
{
	if (dev->tx_pkt_q_tail) {
		dev->tx_pkt_q_tail->next = pkt;
	}
	dev->tx_pkt_q_tail = pkt;

	if (dev->tx_pkt_q_head == NULL) {
		dev->tx_pkt_q_head = pkt;
	}
	dev->tx_pkt_q_count++;
}

static pkt_t *star_nic_tx_pkt_dequeue(struct star_nic_dev_t *dev)
{
	pkt_t *pkt;

	pkt = dev->tx_pkt_q_head;
	if (pkt) {
		dev->tx_pkt_q_head = pkt->next;
		pkt->next = NULL;
		if (dev->tx_pkt_q_head == NULL) {
			dev->tx_pkt_q_tail = NULL;
		}
		dev->tx_pkt_q_count--;
	}

	return pkt;
}

static void star_nic_tx_pkt_requeue(struct star_nic_dev_t *dev, pkt_t *pkt)
{
	pkt->next = dev->tx_pkt_q_head;
	dev->tx_pkt_q_head = pkt;
	if (dev->tx_pkt_q_tail == NULL) {
		dev->tx_pkt_q_tail = pkt;
	}
	dev->tx_pkt_q_count++;
}

static void star_nic_free_mem(void)
{
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	if (tx_desc_pool)
		free(tx_desc_pool);
	if (rx_desc_pool)
		free(rx_desc_pool);
	if (pkt_pool)
		free(pkt_pool);
	if (pkt_buffer_pool)
		free(pkt_buffer_pool);
}

static int star_nic_alloc_mem(void)
{
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
printf("STAR_NIC_MAX_TFD_NUM: %d\n",STAR_NIC_MAX_TFD_NUM);
printf("STAR_NIC_MAX_RFD_NUM: %d\n",STAR_NIC_MAX_RFD_NUM);
#endif
	tx_desc_pool = (TX_DESC_T *)malloc(sizeof(TX_DESC_T) * STAR_NIC_MAX_TFD_NUM + CPU_CACHE_BYTES);
	if (tx_desc_pool == NULL) {
		goto err_out;
	}

	rx_desc_pool = (RX_DESC_T *)malloc(sizeof(TX_DESC_T) * STAR_NIC_MAX_RFD_NUM + CPU_CACHE_BYTES);
	if (rx_desc_pool == NULL) {
		goto err_out;
	}

	pkt_pool = (pkt_t *)malloc(sizeof(pkt_t) * NUM_PKT_BUFFER);
	if (pkt_pool == NULL) {
		goto err_out;
	}

	pkt_buffer_pool = (u8 *)malloc(PKT_BUFFER_ALLOC_SIZE * NUM_PKT_BUFFER);
	if (pkt_buffer_pool == NULL) {
		goto err_out;
	}

	mem_alloc_ok = 1;

	return 0;

err_out:
	star_nic_free_mem();
	return -1;
}

static int star_nic_init_mem(struct star_nic_dev_t *dev)
{
	int i;
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif

	dev->tx_desc_pool_dma = (TX_DESC_T *)CPU_CACHE_ALIGN((u32)tx_desc_pool);
	dev->rx_desc_pool_dma = (RX_DESC_T *)CPU_CACHE_ALIGN((u32)rx_desc_pool);
	memset(dev->tx_desc_pool_dma, 0, sizeof(TX_DESC_T) * STAR_NIC_MAX_TFD_NUM);
	memset(dev->rx_desc_pool_dma, 0, sizeof(RX_DESC_T) * STAR_NIC_MAX_RFD_NUM);
	dev->tx_desc_pool_dma[STAR_NIC_MAX_TFD_NUM - 1].end_of_descriptor = 1;
	dev->rx_desc_pool_dma[STAR_NIC_MAX_RFD_NUM - 1].end_of_descriptor = 1;

	memset(dev->tx_ring_pkt, 0, sizeof(pkt_t *) * STAR_NIC_MAX_TFD_NUM);
	memset(dev->rx_ring_pkt, 0, sizeof(pkt_t *) * STAR_NIC_MAX_RFD_NUM);

	dev->pkt_pool = pkt_pool;
	memset(dev->pkt_pool, 0, sizeof(pkt_t) * NUM_PKT_BUFFER);

	dev->pkt_buffer_pool = pkt_buffer_pool;

	dev->free_pkt_list = &dev->pkt_pool[0];
	for (i = 0; i < (NUM_PKT_BUFFER - 1); i++) {
		dev->pkt_pool[i].next = &dev->pkt_pool[i + 1];
		dev->pkt_pool[i].pkt_buffer = dev->pkt_buffer_pool + (i * PKT_BUFFER_ALLOC_SIZE);
	}
	dev->pkt_pool[i].next = NULL;
	dev->pkt_pool[i].pkt_buffer = dev->pkt_buffer_pool + (i * PKT_BUFFER_ALLOC_SIZE);

	for (i = 0; i < STAR_NIC_MAX_TFD_NUM; i++) {
		dev->tx_desc_pool_dma[i].cpu_own = 1;
		dev->tx_desc_pool_dma[i].ip_csum_offload = 0;
		/* the UDP packet issued by u-boot will not contain UDP checksum, do HW CSUM ? */
		/* it seems that if we enable udp csum, having chance to receive bad csum packet */
		dev->tx_desc_pool_dma[i].udp_csum_offload = 0;
		dev->tx_desc_pool_dma[i].tcp_csum_offload = 0;
	}
	
	for (i = 0; i < STAR_NIC_MAX_RFD_NUM; i++) {
		dev->rx_ring_pkt[i] = star_nic_alloc_pkt(dev);
		dev->rx_desc_pool_dma[i].data_ptr = (u32)dev->rx_ring_pkt[i]->pkt_buffer;
		dev->rx_desc_pool_dma[i].length = PKT_BUFFER_SIZE;
		dev->rx_desc_pool_dma[i].cpu_own = 0;
	}

	dev->tx_pkt_q_head = NULL;
	dev->tx_pkt_q_tail = NULL;

	dev->cur_tx_desc_idx	= 0;
	dev->cur_rx_desc_idx	= 0;
	dev->tx_pkt_count	= 0;
	dev->rx_pkt_count	= 0;
	dev->free_pkt_count	= NUM_PKT_BUFFER;
	dev->tx_pkt_q_count	= 0;

#ifdef STAR_NIC_DEBUG
	printf("TX Descriptor DMA Start:	0x%08x\n", dev->tx_desc_pool_dma);
	printf("RX Descriptor DMA Start:	0x%08x\n", dev->rx_desc_pool_dma);
#if 0
	for (i = 0; i < NUM_PKT_BUFFER; i++) {
		printf("PKT Buffer[%03d]:	0x%08x\n", i, dev->pkt_pool[i].pkt_buffer);
	}
#endif
#endif

	return 0;
}

static int star_nic_dma_config(struct star_nic_dev_t *dev)
{
	u32 volatile dma_config = 0;

	dma_config = NIC_DMA_CONFIG_REG;

#if 0
	/* Config TX DMA */ 
	dma_config &=  ~(0x3 << 6); //TX auto polling :1  us
	//dma_config |=  (0x1 << 6); //TX auto polling :10 us
	dma_config |=  (0x2 << 6); //TX auto polling :100us
	//dma_config |=  (0x3 << 6); //TX auto polling :1000us
	dma_config |=  (0x1 << 5); //TX auto polling C-bit enable
	dma_config &=  ~(0x1 << 4); //TX can transmit packets,No suspend

	/* Config RX DMA */
	dma_config &=  ~(0x3 << 2); //RX auto polling :1  us
	//dma_config |=  (0x1 << 2); //RX auto polling :10 us
	dma_config |=  (0x2 << 2); //RX auto polling :100us
	//dma_config |=  (0x3 << 2); //RX auto polling :1000us
	dma_config |=  (0x1 << 1); //RX auto polling C-bit enable
	dma_config &=  ~0x1; //RX can receive packets, No suspend
#endif

	// 4N(for U-BOOT)
	dma_config |= (0x1 << 16);

	// 4N+2(for Linux)
	//dma_config &= ~(0x1 << 16);

	NIC_DMA_CONFIG_REG = dma_config;

	return 0;
}

static int star_nic_mac_config(struct star_nic_dev_t *dev)
{
	u32 mac_config;

	mac_config = NIC_MAC_CONTROL_REG;

	//mac_config |= (0x1 << 26);	// TX CSUM offload on
	mac_config &= ~(0x1 << 26);	// TX CSUM offload off

	// Rx ChkSum offload On: TCP/UDP/IP */
	//mac_config |= (0x1 << 25);	// RX CSUM offload on
	mac_config &= ~(0x1 << 25);	// RX CSUM offload off

	mac_config |= (0x1 << 24);	// Accept CSUM error packet 
	//mac_config &= ~(0x1 << 24);	// Discard CSUM error packet

	//mac_config |= (0x1 << 23);	// IST Enable
	mac_config &= ~(0x1 << 23);	// IST disable

	mac_config |= (0x1 << 22);	// Strip vlan tag
	//mac_config &= ~(0x1 << 22);	// Keep vlan tag, No Strip

	//mac_config |= (0x1 << 21);	// Accept CRC error pkt
	mac_config &= ~(0x1 << 21);	// Discard CRC error pkt

	mac_config |= (0x1 << 20);	// CRC strip
	//mac_config &= ~(0x1 << 20);	// Keep CRC, No Strip

	//mac_config |= (0x1 << 18);	// Accept oversize pkt
	mac_config &= ~(0x1 << 18);	// Discard oversize pkt

	mac_config &= ~(0x3 << 16);	// clear, set 1518
	mac_config |= (0x2 << 16);	// set 1536

	// IPG
	mac_config |= (0x1f << 10);

	// Do not skip 16 consecutive collisions pkt
	mac_config |= (0x1 << 9);	// allow to re-tx
	//mac_config &= ~(0x1 << 9);	// drop pkt

	mac_config |= (0x1 << 8);	// Fast retry
	//mac_config &= ~(0x1 << 8);	// standard

	NIC_MAC_CONTROL_REG = mac_config;

	return 0;
}

static int star_nic_fc_config(struct star_nic_dev_t *dev)
{
	u32 fc_config;

	fc_config = NIC_FLOW_CONTROL_CONFIG_REG;

	// send pause on frame threshold
	fc_config &= ~(0xfff << 16);	// Clear
	fc_config |= (0x360 << 16);	// Set

	//fc_config |= (0x1 << 8);	// Enable UC_PAUSE
	fc_config &= ~(0x1 << 8);	// Disable UC_PAUSE

	fc_config |= (0x1 << 7);	// Enable Half Duplex backpressure
	//fc_config &= ~(0x1 << 7);	// Disable Half Duplex backpressure

    	//fc_config |= (0x1 << 6);	// CRS-based BP
	fc_config &= ~(0x1 << 6);	// Collision-based BP

	/* max BP collision enable */
	//fc_config |= (0x1 << 5);	// Enable max BP collision
	fc_config &= ~(0x1 << 5);	// Disable max BP collision

	// max BP collision count
	fc_config &= ~(0x1f);		// Clear
	fc_config |= (0xc);		// Set

	NIC_FLOW_CONTROL_CONFIG_REG = fc_config;

	return 0;
}

static int star_nic_phy_config(struct star_nic_dev_t *dev)
{
	u32 phy_addr;
	u32 phy_config;

#if 1 // on ASIC
	phy_addr = 0;
#else // on FPGA
	phy_addr = 1;
#endif

#ifdef CONFIG_STAR_NIC_PHY_INTERNAL
	u16 phy_data;

	dev->phy_addr = STAR_NIC_PHY_ADDR;
	// set phy addr for auto-polling
	phy_config |= (dev->phy_addr & 0x1f) << 24;

	// set internal phy mode
	// internel 10/100 phy
	phy_config |= 0x1 << 18;

	// MII
	phy_config &= ~(0x1 << 17);

	// MAC mode
	phy_config &= ~(0x1 << 16);
#endif // CONFIG_STAR_NIC_PHY_INTERNAL

#ifdef CONFIG_STAR_NIC_PHY_VSC8601
	u16 phy_data;

	phy_config = NIC_PHY_CONTROL_REG1;

	// phy addr for auto-polling
	phy_config |= ((phy_addr & 0x1f) << 24);

	// set external phy mode
	phy_config &= ~(0x1 << 18);

	// set RGMII
	phy_config |= (0x1 << 17);

	// set MII interface
	phy_config &= ~(0x1 << 16);

	NIC_PHY_CONTROL_REG1 = phy_config;

	dev->phy_addr = STAR_NIC_PHY_ADDR;
	// set phy addr for auto-polling
	phy_config |= (dev->phy_addr & 0x1f) << 24;

	// set external phy mode
	// MII/RGMII interface
	phy_config &= ~(0x1 << 18);

	// RGMII
	phy_config |= (0x1 << 17);

	// MAC mode
	phy_config &= ~(0x1 << 16);

	star_nic_read_phy(dev->phy_addr, 3, &phy_data);
	if ((phy_data & 0x000f) == 0x0000) { // type A chip
		u16 tmp16;

		printf("VSC8601 Type A Chip\n");
		star_nic_write_phy(dev->phy_addr, 31, 0x52B5);
		star_nic_write_phy(dev->phy_addr, 16, 0xAF8A);

		phy_data = 0x0;
		star_nic_read_phy(dev->phy_addr, 18, &tmp16);
		phy_data |= (tmp16 & ~0x0);
		star_nic_write_phy(dev->phy_addr, 18, phy_data);

		phy_data = 0x0008;
		star_nic_read_phy(dev->phy_addr, 17, &tmp16);
		phy_data |= (tmp16 & ~0x000C);
		star_nic_write_phy(dev->phy_addr, 17, phy_data);        	

		star_nic_write_phy(dev->phy_addr, 16, 0x8F8A);        	

            	star_nic_write_phy(dev->phy_addr, 16, 0xAF86);        	

		phy_data = 0x0008;
		star_nic_read_phy(dev->phy_addr, 18, &tmp16);
		phy_data |= (tmp16 & ~0x000C);
		star_nic_write_phy(dev->phy_addr, 18, phy_data);        	

		phy_data = 0x0;
		star_nic_read_phy(dev->phy_addr, 17, &tmp16);
		phy_data |= (tmp16 & ~0x0);
		star_nic_write_phy(dev->phy_addr, 17, phy_data);        	

		star_nic_write_phy(dev->phy_addr, 16, 0x8F8A);        	

		star_nic_write_phy(dev->phy_addr, 16, 0xAF82);        	

		phy_data = 0x0;
		star_nic_read_phy(dev->phy_addr, 18, &tmp16);
		phy_data |= (tmp16 & ~0x0);
		star_nic_write_phy(dev->phy_addr, 18, phy_data);        	

		phy_data = 0x0100;
		star_nic_read_phy(dev->phy_addr, 17, &tmp16);
		phy_data |= (tmp16 & ~0x0180);
		star_nic_write_phy(dev->phy_addr, 17, phy_data);        	

		star_nic_write_phy(dev->phy_addr, 16, 0x8F82);        	

		star_nic_write_phy(dev->phy_addr, 31, 0x0);        	
            
		//Set port type: single port
		star_nic_read_phy(dev->phy_addr, 9, &phy_data);        	
		phy_data &= ~( 0x1 << 10);
		star_nic_write_phy(dev->phy_addr, 9, phy_data);        	
	} else if ((phy_data & 0x000f) == 0x0001) { // type B chip
		printf("VSC8601 Type B Chip\n");
		star_nic_read_phy(dev->phy_addr, 23, &phy_data);
		phy_data |= ( 0x1 << 8); //set RGMII timing skew
		star_nic_write_phy(dev->phy_addr, 23, phy_data);
	}

	// change to extended registers
	star_nic_write_phy(dev->phy_addr, 31, 0x0001);

	star_nic_read_phy(dev->phy_addr, 28, &phy_data);
	phy_data &= ~(0x3 << 14); // set RGMII TX timing skew
	phy_data |= (0x3 << 14); // 2.0ns
	phy_data &= ~(0x3 << 12); // set RGMII RX timing skew
	phy_data |= (0x3 << 12); // 2.0ns
	star_nic_write_phy(dev->phy_addr, 28, phy_data);

	// change to normal registers
	star_nic_write_phy(dev->phy_addr, 31, 0x0000);

	// no need to set the MAC clock skew when the phy is
	// VSC8601, or will get crc error
	//NIC_TEST_0_REG = (0x1 << 2) | (0x0 << 0);
#endif // CONFIG_STAR_NIC_PHY_VSC8601

#ifdef CONFIG_STAR_NIC_PHY_IP101A
	dev->phy_addr = 1;
	// set phy addr for auto-polling
	phy_config |= (dev->phy_addr & 0x1f) << 24;

	// set external phy mode
	// MII/RGMII interface
	phy_config &= ~(0x1 << 18);

	// MII
	phy_config &= ~(0x1 << 17);

	// MAC mode
	phy_config &= ~(0x1 << 16);
#endif // CONFIG_STAR_NIC_PHY_IP101A

#ifdef CONFIG_STAR_NIC_PHY_IP1001
	u16 phy_data;
	phy_addr = 1;
	
	phy_config = NIC_PHY_CONTROL_REG1;
	
		// set phy addr for auto-polling
		phy_config |= ((phy_addr & 0x1f) << 24);
	
		// set external phy mode
		// MII/RGMII interface
		phy_config &= ~(0x1 << 18);
	
		// RGMII
		phy_config |= (0x1 << 17);
	
		// MAC mode
		phy_config &= ~(0x1 << 16);
	
	 NIC_PHY_CONTROL_REG1 = phy_config;
		star_nic_read_phy(phy_addr,2,&phy_data);
		//printf("\n phy.reg2=0x%04x",phy_data);
		
#if 1//set AN capability
	
		star_nic_read_phy(phy_addr,4,&phy_data);
	
		phy_data &= ~(0xf<<5);//clear
		phy_data |= (0x1<<5); //10Half
		phy_data |= (0x1<<6); //10Full
		phy_data |= (0x1<<7); //100Half
		phy_data |= (0x1<<8); //100Full
	//	  phy_data &= ~(0x1<<10); //FC off
		phy_data |= (0x1<<10); //FC on
		star_nic_write_phy(phy_addr,4,phy_data);
	
		star_nic_read_phy(phy_addr,9,&phy_data);
	
		phy_data |= (0x1<<9); //1000Full on
	
		phy_data &= ~(0x1<<10); 
	
		phy_data |= (0x1<<12); 
	
		star_nic_write_phy(phy_addr,9,phy_data);
	
		star_nic_read_phy(phy_addr,16,&phy_data);
	
		phy_data &= ~(0x1<<11); //Smart function off
	
		phy_data |=  (0x1<<0); //TX delay
	
		phy_data |=  (0x1<<1); //RX delay
	
		star_nic_write_phy(phy_addr,16,phy_data);
	
		star_nic_read_phy(phy_addr,16,&phy_data);
		//printf("\n phy.reg16=0x%04x",phy_data);
	
	//	  Hal_Nic_Read_PHY(NIC_PHY_ADDRESS,20,&phy_data);
	//
	//	  phy_data &= ~(0x1<<2); 
	//
	//	  phy_data |=  (0x1<<9); 
	//	  Hal_Nic_Write_PHY(NIC_PHY_ADDRESS,20,phy_data);
	
		star_nic_read_phy(phy_addr,0,&phy_data);
		phy_data |= (0x1<<9); //re-AN
		star_nic_write_phy(phy_addr,0,phy_data);
	
		star_nic_read_phy(phy_addr,9,&phy_data);
		//printf("\n phy.reg9=0x%04x",phy_data);	  
#endif	
#endif // CONFIG_STAR_NIC_PHY_IP1001

	// Enable PHY AN mode
	phy_config |= (0x1 << 8); //AN On
	//phy_config &= ~(0x1 << 8); //AN off

	if (!((phy_config >> 8) & 0x1)) { //AN disbale
		// Force to FullDuplex mode
		phy_config &= ~(0x1 << 11); //Half

		// Force to 100Mbps mode
		phy_config &= ~(0x3 << 9); // clear to 10M
		phy_config |= (0x1 << 9); // set to 100M
	}

	// Force TX FlowCtrl On,in 1000M
	phy_config |= (0x1 << 13);

	// Force TX FlowCtrl On, in 10/100M
	phy_config |= (0x1 << 12);

	// Enable MII auto polling
	phy_config &= ~(0x1 << 7); // auto-polling enable
	//phy_config |= (0x1 << 7); // auto-polling disable

	NIC_PHY_CONTROL_REG1 = phy_config;

	return 0;
}

static int star_nic_vlan_config(struct star_nic_dev_t *dev)
{
	u32 vlan_id;

	//1.Setup MyVLAN ID0_1
	vlan_id  = 0; //clear
	vlan_id |= (my_vlan_id[0].vid & 0x0fff);
	vlan_id |= ((my_vlan_id[1].vid & 0x0fff) << 16);
	NIC_MY_VLANID_0_1 = vlan_id;

	//2.Setup MyVLAN ID2_3
	vlan_id  = 0; //clear
	vlan_id |= (my_vlan_id[2].vid & 0x0fff);
	vlan_id |= ((my_vlan_id[3].vid & 0x0fff) << 16);
	NIC_MY_VLANID_2_3 = vlan_id;

	//3.Setup vlan_id control bits
	NIC_MY_VLANID_CONTROL_REG =
		((my_vlan_id[0].control << 0) |
		 (my_vlan_id[1].control << 1) |
		 (my_vlan_id[2].control << 2) |
		 (my_vlan_id[3].control << 3));

	return 0;
}

static int star_nic_arl_config(struct star_nic_dev_t *dev)
{
	u32 arl_config;

	arl_config = NIC_ARL_CONFIG_REG;
	arl_config |= (0x1 << 4); // Misc Mode ON
	//arl_config &= ~(0x1 << 4); // Misc Mode Off
	arl_config |= (0x1 << 3); // My MAC only enable
	arl_config &= ~(0x1 << 2); // Learn SA On
	arl_config &= ~(0x1 << 1); // Forward MC to CPU
	arl_config &= ~(0x1); // Hash direct mode
	NIC_ARL_CONFIG_REG = arl_config;

	return 0;
}

static void star_nic_mac_addr_assign(struct star_nic_dev_t *dev)
{
	int reg;
	int env_size;
	char *e, *s;
	char s_env_mac[64];
	char v_env_mac[6];
	char *mac_addr;

	env_size = getenv_r("ethaddr", s_env_mac, sizeof(s_env_mac));

	if (env_size > 0) {
		s = s_env_mac;
		for (reg = 0; reg < 6; reg++) {
			v_env_mac[reg] = s ? simple_strtoul(s, &e, 16) : 0;
			if (s) {
				s = (*e) ? (e + 1) : e;
			}
		}
		mac_addr = v_env_mac;
	} else {
		mac_addr = default_mac_addr;
	}

	memcpy(dev->mac, mac_addr, 6);
}

static void star_nic_mac_addr_config(struct star_nic_dev_t *dev)
{
	star_nic_mac_addr_assign(dev);

	NIC_MY_MAC_HIGH_BYTE_REG =
		((dev->mac[0] & 0xff) << 16) |
		((dev->mac[1] & 0xff));
	NIC_MY_MAC_LOW_BYTE_REG =
	       	((dev->mac[2] & 0xff) << 24) |
		((dev->mac[3] & 0xff) << 16) |
		((dev->mac[4] & 0xff) << 8)  |
		((dev->mac[5] & 0xff));

#ifdef STAR_NIC_DEBUG
	printf("MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
		dev->mac[0],
		dev->mac[1],
		dev->mac[2],
		dev->mac[3],
		dev->mac[4],
		dev->mac[5]);
#endif
}

static int star_nic_hw_init(struct star_nic_dev_t *dev)
{
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	// set hight
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 15);
	// set low
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << 15);
	// set high
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 15);

	/* NIC software reset */
	// set hight
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 5);
	// set low
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << 5);
	// set high
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 5);

#if 1
	// 125MHz as NIC clock
	PWRMGT_SYSTEM_CLOCK_CONTROL_REG &= ~(0x1 << 7);
#else
	// 62.5MHz as NIC clock
	PWRMGT_SYSTEM_CLOCK_CONTROL_REG |= (0x1 << 7);
#endif

	// enable NIC clock
	HAL_PWRMGT_ENABLE_NIC_CLOCK();
	udelay(100);
	NIC_MAC_CONTROL_REG &= ~((u32)0x3 << 30);

	// Configure GPIO for NIC MDC/MDIO pins
	HAL_MISC_ENABLE_MDC_MDIO_PINS();
	HAL_MISC_ENABLE_NIC_COL_PINS();

#ifdef CONFIG_STAR_NIC_PHY_INTERNAL
        MISC_GPIOA_PIN_ENABLE_REG |= (0x7 << 22);
        MISC_FAST_ETHERNET_PHY_CONFIG_REG |=  (FE_PHY_LED_MODE >> 12) & 0x3;

	// set hight
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 15);
	// set low
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << 15);
	// set high
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << 15);
#endif

	HAL_NIC_DISABLE_ALL_INTERRUPT_STATUS_SOURCES();
	HAL_NIC_CLEAR_ALL_INTERRUPT_STATUS_SOURCES();
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_NIC_TXTC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_NIC_TXTC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_NIC_RXRC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_NIC_RXRC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_NIC_TXQE_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_NIC_TXQE_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_NIC_RXQF_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_NIC_RXQF_BIT_INDEX);

	star_nic_mac_config(dev);
	star_nic_fc_config(dev);
	if (star_nic_phy_config(dev) != 0) {
		return -1;
	}
	star_nic_vlan_config(dev);
	star_nic_arl_config(dev);
	star_nic_mac_addr_config(dev);

	*(u32 volatile *)(0x76000000+0x0c) = 0x00000125;    //0x00000105 pb0_nic

	HAL_NIC_TX_DMA_STOP();
	HAL_NIC_RX_DMA_STOP();

	HAL_NIC_WRITE_TXSD((u32)dev->tx_desc_pool_dma);
	HAL_NIC_WRITE_TX_BASE((u32)dev->tx_desc_pool_dma);
	HAL_NIC_WRITE_RXSD((u32)dev->rx_desc_pool_dma);
	HAL_NIC_WRITE_RX_BASE((u32)dev->rx_desc_pool_dma);

	// enable delayed interrupt
	NIC_DELAYED_INT_CONFIG_REG = (1 << 16) | ((0x20 & 0xFF) << 8) | (0x80 & 0xFF);

	star_nic_dma_config(dev);

	return 0;
}

void star_flush_rx_ring(){
	struct star_nic_dev_t *dev = &star_nic_dev;
	u32 last_time;
	u32 i;
	RX_DESC_T volatile *rx_desc;
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	for(i=0;i<STAR_NIC_MAX_RFD_NUM;i++){
		rx_desc = &dev->rx_desc_pool_dma[dev->cur_rx_desc_idx];
		if(!rx_desc->cpu_own) break;
		rx_desc->length = PKT_BUFFER_SIZE;
		rx_desc->cpu_own=0;
			if (dev->cur_rx_desc_idx == (STAR_NIC_MAX_RFD_NUM - 1)) {
			dev->cur_rx_desc_idx = 0;
		} else {
			dev->cur_rx_desc_idx++;
		}
	}

}

static int star_nic_check_link(struct star_nic_dev_t *dev)
{
	u32 last_time;
	u16 phy_data;

#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	reset_timer_masked();
	last_time = get_timer_masked();
	while (1) {
		star_nic_read_phy(dev->phy_addr, 0x1, &phy_data);
		if  ((phy_data >> 2) & 0x1) {
			printf("Up\n");
			break;
		}
		if ((get_timer_masked() - last_time) > (1 * CFG_HZ)) {
			last_time = get_timer_masked();
			printf(".");
		}
		if (get_timer_masked() > (10 * CFG_HZ)) {
			printf("Down\n");
			return 1;
		}
		udelay(1000);
	}

#if 0
	printf("Waiting %d seconds ", WAIT_FOR_LINK);
	reset_timer_masked();
	last_time = get_timer_masked();
	while (1) {
		if (get_timer_masked() - last_time > (1 * CFG_HZ)) {
			last_time = get_timer_masked();
			printf(".");
		}
	star_flush_rx_ring();
		if (get_timer_masked() > (WAIT_FOR_LINK * CFG_HZ)) {
			break;
		}
	}
	printf("\n");
#endif

	return 0;
}

#ifdef STAR_NIC_DEBUG
static void star_nic_mib_reset(struct star_nic_dev_t *dev)
{
	u32 v;

	v = NIC_MIB_RX_OK_PKT_CNTR;
	v = NIC_MIB_RX_OK_BYTE_CNTR;
	v = NIC_MIB_RX_RUNT_BYTE_CNTR;
	v = NIC_MIB_RX_OSIZE_DROP_PKT_CNTR;
	v = NIC_MIB_RX_NO_BUF_DROP_PKT_CNTR;
	v = NIC_MIB_RX_CRC_ERR_PKT_CNTR;
	v = NIC_MIB_RX_ARL_DROP_PKT_CNTR;
	v = NIC_MIB_MYVLANID_MISMATCH_DROP_PKT_CNTR;
	v = NIC_MIB_RX_CHKSUM_ERR_PKT_CNTR;
	v = NIC_MIB_RX_PAUSE_FRAME_PKT_CNTR;
	v = NIC_MIB_TX_OK_PKT_CNTR;
	v = NIC_MIB_TX_OK_BYTE_CNTR;
	v = NIC_MIB_TX_PAUSE_FRAME_CNTR;

	dev->mib_info.mib_rx_ok_pkt		+= 0;
	dev->mib_info.mib_rx_ok_byte		+= 0;
	dev->mib_info.mib_rx_runt		+= 0;
	dev->mib_info.mib_rx_over_size		+= 0;
	dev->mib_info.mib_rx_no_buffer_drop	+= 0;
	dev->mib_info.mib_rx_crc_err		+= 0;
	dev->mib_info.mib_rx_arl_drop		+= 0;
	dev->mib_info.mib_rx_myvid_drop		+= 0;
	dev->mib_info.mib_rx_csum_err		+= 0;
	dev->mib_info.mib_rx_pause_frame	+= 0;
	dev->mib_info.mib_tx_ok_pkt		+= 0;
	dev->mib_info.mib_tx_ok_byte		+= 0;
	dev->mib_info.mib_tx_pause_frame	+= 0;
}

static void star_nic_mib_read(struct star_nic_dev_t *dev)
{
	dev->mib_info.mib_rx_ok_pkt		+= NIC_MIB_RX_OK_PKT_CNTR;
	dev->mib_info.mib_rx_ok_byte		+= NIC_MIB_RX_OK_BYTE_CNTR;
	dev->mib_info.mib_rx_runt		+= NIC_MIB_RX_RUNT_BYTE_CNTR;
	dev->mib_info.mib_rx_over_size		+= NIC_MIB_RX_OSIZE_DROP_PKT_CNTR;
	dev->mib_info.mib_rx_no_buffer_drop	+= NIC_MIB_RX_NO_BUF_DROP_PKT_CNTR;
	dev->mib_info.mib_rx_crc_err		+= NIC_MIB_RX_CRC_ERR_PKT_CNTR;
	dev->mib_info.mib_rx_arl_drop		+= NIC_MIB_RX_ARL_DROP_PKT_CNTR;
	dev->mib_info.mib_rx_myvid_drop		+= NIC_MIB_MYVLANID_MISMATCH_DROP_PKT_CNTR;
	dev->mib_info.mib_rx_csum_err		+= NIC_MIB_RX_CHKSUM_ERR_PKT_CNTR;
	dev->mib_info.mib_rx_pause_frame	+= NIC_MIB_RX_PAUSE_FRAME_PKT_CNTR;
	dev->mib_info.mib_tx_ok_pkt		+= NIC_MIB_TX_OK_PKT_CNTR;
	dev->mib_info.mib_tx_ok_byte		+= NIC_MIB_TX_OK_BYTE_CNTR;
	dev->mib_info.mib_tx_pause_frame	+= NIC_MIB_TX_PAUSE_FRAME_CNTR;
}

static void star_nic_mib_show(struct star_nic_dev_t *dev)
{
	printf("mib_rx_ok_pkt:		%ld\n", dev->mib_info.mib_rx_ok_pkt);
	printf("mib_rx_ok_byte:		%ld\n", dev->mib_info.mib_rx_ok_byte);
	printf("mib_rx_runt:		%ld\n", dev->mib_info.mib_rx_runt);
	printf("mib_rx_over_size	%ld\n", dev->mib_info.mib_rx_over_size);
	printf("mib_rx_no_buffer_drop:	%ld\n", dev->mib_info.mib_rx_no_buffer_drop);
	printf("mib_rx_crc_err:		%ld\n", dev->mib_info.mib_rx_crc_err);
	printf("mib_rx_arl_drop:	%ld\n", dev->mib_info.mib_rx_arl_drop);
	printf("mib_rx_myvid_drop:	%ld\n", dev->mib_info.mib_rx_myvid_drop);
	printf("mib_rx_csum_err:	%ld\n", dev->mib_info.mib_rx_csum_err);
	printf("mib_rx_pause_frame:	%ld\n", dev->mib_info.mib_rx_pause_frame);
	printf("mib_tx_ok_pkt:		%ld\n", dev->mib_info.mib_tx_ok_pkt);
	printf("mib_tx_ok_byte:		%ld\n", dev->mib_info.mib_tx_ok_byte);
	printf("mib_tx_pause_frame:	%ld\n", dev->mib_info.mib_tx_pause_frame);
}
#endif

static void star_nic_halt(struct star_nic_dev_t *dev)
{
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	HAL_NIC_RX_DMA_STOP();
	HAL_NIC_TX_DMA_STOP();
}

#ifdef STAR_NIC_DEBUG
void print_nic_debug(){
	struct star_nic_dev_t *dev = &star_nic_dev;
	RX_DESC_T volatile *rx_desc;
	TX_DESC_T volatile *tx_desc;
	u32 i;
	star_nic_show_reg();
	printf("dev->cur_tx_desc_idx: %d\n", dev->cur_tx_desc_idx);
	printf("dev->cur_rx_desc_idx: %d\n", dev->cur_rx_desc_idx);
	printf("TX PACKET COUNT: %d\n", dev->tx_pkt_count);
	printf("RX PACKET COUNT: %d\n", dev->rx_pkt_count);
	star_nic_mib_read(dev);
	star_nic_mib_show(dev);

	if(dev->rx_desc_pool_dma){
		for(i=0;i<STAR_NIC_MAX_RFD_NUM;i++){
			rx_desc = &dev->rx_desc_pool_dma[i];	
			printf("rx desc %2.2d cpu_own=%d\n",i,rx_desc->cpu_own);
		}
	}else{
		printf("rx desc pool not initialized!!\n");
	}
	printf("==================\n");
	if(dev->tx_desc_pool_dma){
		for(i=0;i<STAR_NIC_MAX_TFD_NUM;i++){
			tx_desc = &dev->tx_desc_pool_dma[i];	
			printf("tx desc %2.2d cpu_own=%d\n",i,tx_desc->cpu_own);
}
	}else{
		printf("rx desc pool not initialized!!\n");
	}
}
#endif

static int star_nic_init(struct star_nic_dev_t *dev)
{
	int err;
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif

	if (!mem_alloc_ok) {
		err = star_nic_alloc_mem();
		if (err) {
			return err;
		}
	}

	star_nic_init_mem(dev);

	err = star_nic_hw_init(dev);
	if (err) {
		star_nic_halt(dev);
		memset(dev, 0, sizeof(struct star_nic_dev_t));
		return err;
	}
#ifdef STAR_NIC_DEBUG
debug_last_time = 0xffffffff;
#endif
	return 0;
}

static void star_nic_open(struct star_nic_dev_t *dev)
{
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	HAL_NIC_RX_DMA_START();
	star_nic_phy_powerup(dev);
#ifdef CONFIG_STAR_NIC_PHY_INTERNAL
	internal_phy_patch_check();
#endif
}

static void star_nic_close(struct star_nic_dev_t *dev)
{
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif
	star_nic_phy_powerdown(dev);
	star_nic_halt(dev);
}

static void star_nic_rx(struct star_nic_dev_t *dev)
{
	RX_DESC_T volatile *rx_desc;
	pkt_t *rcvpkt;
	pkt_t *newpkt;
	u32 rxcount = 0;
	u32 first_time=1;
	while (1) {
		rx_desc = &dev->rx_desc_pool_dma[dev->cur_rx_desc_idx];
		if (rx_desc->cpu_own == 0) {
			if(first_time) {
				HAL_NIC_RX_DMA_START();
				first_time=0;
				continue;
			}
			break;
		}
		rcvpkt = dev->rx_ring_pkt[dev->cur_rx_desc_idx];
		rcvpkt->length = rx_desc->length;
		newpkt = star_nic_alloc_pkt(dev);
		if (newpkt == NULL) {
			printf("Allocate pkt failed on RX...\n");
		}
		dev->rx_ring_pkt[dev->cur_rx_desc_idx] = newpkt;
		rx_desc->data_ptr = (u32)newpkt->pkt_buffer;
		rx_desc->length = PKT_BUFFER_SIZE;
		rx_desc->cpu_own = 0;
#ifdef STAR_NIC_DEBUG
		printf("RX PKT buffer: 0x%08x\n", rcvpkt->pkt_buffer);
		{
			int i=0;
			printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n", rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++]);
			printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n", rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++]);
			printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n", rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++]);
			printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n", rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++], rcvpkt->pkt_buffer[i++]);
		}
		printf("RX PKT length: %d\n", rcvpkt->length);
		
#endif
		NetReceive(rcvpkt->pkt_buffer, rcvpkt->length);
		star_nic_free_pkt(dev, rcvpkt);

		if (dev->cur_rx_desc_idx == (STAR_NIC_MAX_RFD_NUM - 1)) {
			dev->cur_rx_desc_idx = 0;
		} else {
			dev->cur_rx_desc_idx++;
		}

		rxcount++;
		if (rxcount == STAR_NIC_MAX_RFD_NUM) {
			break;
		}
	}

	dev->rx_pkt_count += rxcount;
}

static void star_nic_tx(struct star_nic_dev_t *dev)
{
	TX_DESC_T volatile *tx_desc;
	pkt_t *pkt;
	u32 txcount = 0;
#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif

	while ((pkt = star_nic_tx_pkt_dequeue(dev))) {
		tx_desc = &dev->tx_desc_pool_dma[dev->cur_tx_desc_idx];
		if (!tx_desc->cpu_own) {
			star_nic_tx_pkt_requeue(dev, pkt);
			break;
		} else {
			if (dev->tx_ring_pkt[dev->cur_tx_desc_idx]) {
				star_nic_free_pkt(dev, dev->tx_ring_pkt[dev->cur_tx_desc_idx]);
			}
		}
#ifdef STAR_NIC_DEBUG
		//printf("TX PKT buffer: 0x%08x\n", pkt->pkt_buffer);
		//printf("TX PKT length: %d\n", pkt->length);
#endif
		dev->tx_ring_pkt[dev->cur_tx_desc_idx] = pkt;
		tx_desc->data_ptr = (u32)pkt->pkt_buffer;
		tx_desc->length = pkt->length;
		tx_desc->cpu_own = 0;
		tx_desc->first_segment = 1;
		tx_desc->last_segment = 1;

		if (dev->cur_tx_desc_idx == (STAR_NIC_MAX_TFD_NUM - 1)) {
			dev->cur_tx_desc_idx = 0;
		} else {
			dev->cur_tx_desc_idx++;
		}

		txcount++;
	}

	if (txcount) {
		dev->tx_pkt_count += txcount;

		HAL_NIC_TX_DMA_START();
	}
}

int star_eth_hw_init(){
	struct star_nic_dev_t *dev = &star_nic_dev;

//	star_nic_hw_init(dev);
//	star_nic_open(dev);
//	HAL_NIC_RX_DMA_START();
//	star_nic_phy_powerup(dev);
	star_nic_phy_powerdown(dev);
//	star_nic_halt(dev);
}

int eth_init(bd_t *bis)
{
	struct star_nic_dev_t *dev = &star_nic_dev;
	int err = 0;

#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif

	if (star_nic_dev_initialized) {
		star_nic_open(dev);
		if (star_nic_check_link(dev) == 0) {
			return 0;
		} else {
			star_nic_close(dev);
			return -1;
		}
	} 

	if (star_nic_init(&star_nic_dev) == 0) {
		star_nic_dev_initialized = 1;
		star_nic_open(dev);
		if (star_nic_check_link(dev) == 0) {
			return 0;
		} else {
			star_nic_close(dev);
			return -1;
		}
	} else {
		err = -1;
	}

	return err;
}

s32 eth_send(volatile void *packet, s32 length)
{
	struct star_nic_dev_t *dev = &star_nic_dev;

	pkt_t *pkt;

	pkt = star_nic_alloc_pkt(dev);
	if (!pkt) {
		printf("Allocate pkt failed on TX...\n");
		return 0;
	}

	memcpy(pkt->pkt_buffer, (void *)packet, length);
	if (length < PKT_MIN_SIZE) {
		pkt->length = PKT_MIN_SIZE;
		memset(pkt->pkt_buffer + length, 0x00, PKT_MIN_SIZE - length);
	} else {
		pkt->length = length;
	}
	star_nic_tx_pkt_enqueue(dev, pkt);
	star_nic_tx(dev);

	return 0;
}

s32 eth_rx(void)
{
	struct star_nic_dev_t *dev = &star_nic_dev;
#ifdef STAR_NIC_DEBUG
	if ((get_timer(0) - debug_last_time) > (3 * CFG_HZ)) {
		print_nic_debug();

		debug_last_time = get_timer(0);
	}
#endif

	star_nic_rx(dev);

	return 0;
}

void eth_halt(void)
{
	struct star_nic_dev_t *dev = &star_nic_dev;

#ifdef STAR_NIC_DEBUG
printf("%s()\n",__FUNCTION__);
#endif

	if (star_nic_dev_initialized) {
		star_nic_close(dev);
	}
}

#endif
