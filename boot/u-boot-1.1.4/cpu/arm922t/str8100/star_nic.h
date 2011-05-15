#ifndef __STAR_NIC_H__
#define __STAR_NIC_H__

// the phy addr of VSC8601 && internal phy
#define STAR_NIC_PHY_ADDR		0

#define STAR_NIC_MAX_TFD_NUM		16
#define STAR_NIC_MAX_RFD_NUM		16

#define PKT_BUFFER_ALLOC_SIZE		1600
#define PKT_BUFFER_SIZE			1536
#define PKT_MIN_SIZE			60

#define NUM_PKT_BUFFER			128

typedef struct
{
	u32	data_ptr;
	u32	length:			16;
	u32	reserved0:		7;
	u32	tcp_csum_offload:	1;
	u32	udp_csum_offload:	1;
	u32	ip_csum_offload:	1;
	u32	insert_vlan_tag:	1;
	u32	interrupt:		1;
	u32	last_segment:		1;
	u32	first_segment:		1;
	u32	end_of_descriptor:	1;
	u32	cpu_own:		1;
	u32	vlan_vid:		12;
	u32	vlan_cfi:		1;
	u32	vlan_priority:		3;
	u32	vlan_epid:		16;
	u32	reserved1;
} __attribute__((packed)) TX_DESC_T;

typedef struct
{
	u32	data_ptr;
	u32	length:			16;
	u32	tcp_udp_csum_fail:	1;
	u32	ip_csum_fail:		1;
	u32	protocol:		2;
	u32	vlan_tagged:		1;
	u32	my_mac:			1;
	u32	hit_hash_table:		1;
	u32	reserved_mc_addr:	1;
	u32	crc_err:		1;
	u32	over_size:		1;
	u32	reserved0:		2;
	u32	last_segment:		1;
	u32	first_segment:		1;
	u32	end_of_descriptor:	1;
	u32	cpu_own:		1;
	u32	vlan_vid:		12;
	u32	vlan_cfi:		1;
	u32	vlan_priority:		3;
	u32	vlan_epid:		16;
	u32	reserved1;
} __attribute__((packed)) RX_DESC_T;

typedef struct pkt
{
	struct pkt	*next;
	u32		flags;
	u32		length;
	u8		*pkt_buffer;
} pkt_t;

typedef struct
{
	u32		mib_rx_ok_pkt;
	u32		mib_rx_ok_byte;
	u32		mib_rx_runt;
	u32		mib_rx_over_size;
	u32		mib_rx_no_buffer_drop;
	u32		mib_rx_crc_err;
	u32		mib_rx_arl_drop;
	u32		mib_rx_myvid_drop;
	u32		mib_rx_csum_err;
	u32		mib_rx_pause_frame;
	u32		mib_tx_ok_pkt;
	u32		mib_tx_ok_byte;
	u32		mib_tx_pause_frame;
} mib_info_t;

struct star_nic_dev_t
{
	TX_DESC_T		*tx_desc_pool_dma;
	RX_DESC_T		*rx_desc_pool_dma;

	pkt_t			*tx_ring_pkt[STAR_NIC_MAX_TFD_NUM];
	pkt_t			*rx_ring_pkt[STAR_NIC_MAX_TFD_NUM];

	u32			cur_tx_desc_idx;
	u32			cur_rx_desc_idx;

	pkt_t			*pkt_pool;
	u8			*pkt_buffer_pool;
	u32			free_pkt_count;
	pkt_t			*free_pkt_list;

	pkt_t			*tx_pkt_q_head;
	pkt_t			*tx_pkt_q_tail;
	u32			tx_pkt_q_count;

	u32			tx_pkt_count;
	u32			rx_pkt_count;

	u8			mac[6];

	u8			phy_addr;
	u16			phy_id;

	mib_info_t		mib_info;
};

#endif

