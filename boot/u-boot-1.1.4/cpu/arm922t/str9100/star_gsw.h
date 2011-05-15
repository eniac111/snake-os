#ifndef __STAR_GSW_H__
#define __STAR_GSW_H__

#define STAR_GSW_MAX_TFD_NUM		16
#define STAR_GSW_MAX_RFD_NUM		16

#define PKT_BUFFER_ALLOC_SIZE		1600
#define PKT_BUFFER_SIZE			1536
#define PKT_MIN_SIZE			60

#define NUM_PKT_BUFFER			128

#define DEFAULT_VLAN_ID			1

#define GSW_VLAN_PORT_0			(1 << 0)
#define GSW_VLAN_PORT_1			(1 << 1)
#define GSW_VLAN_PORT_CPU		(1 << 2)

typedef struct
{
	u32	data_ptr;
	u32	length:			16;
	u32	tcp_csum_offload:	1;
	u32	udp_csum_offload:	1;
	u32	ip_csum_offload:	1;
	u32	port_map:		3;
	u32	force_route:		1;
	u32	forced_priority:	3;
	u32	priority:		1;
	u32	interrupt:		1;
	u32	last_segment:		1;
	u32	first_segment:		1;
	u32	end_of_descriptor:	1;
	u32	cpu_own:		1;
	u32	vlan_group_id:		3;
	u32	insert_vlan_tag:	1;
	u32	pppoe_session_index:	3;
	u32	insert_pppoe_session:	1;
	u32	reserved0:		24;
	u32	reserved1;
} __attribute__((packed)) TX_DESC_T;

typedef struct
{
	u32	data_ptr;
	u32	length:			16;
	u32	tcp_udp_csum_fail:	1;
	u32	ip_csum_fail:		1;
	u32	protocol:		2;
	u32	hnat_reason:		6;
	u32	source_port:		2;
	u32	last_segment:		1;
	u32	first_segment:		1;
	u32	end_of_descriptor:	1;
	u32	cpu_own:		1;
	u32	reserved0;
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
	u16	gid;
	u16	vid;
	u32	port_map;
	u32	tag_port_map;
	u8	mac[6];
} vlan_config_t;

typedef struct
{
	u32	pvid;
	u32	config;
	u32	state;
} port_config_t;

struct star_gsw_dev_t
{
	TX_DESC_T		*tx_desc_pool_dma;
	RX_DESC_T		*rx_desc_pool_dma;

	pkt_t			*tx_ring_pkt[STAR_GSW_MAX_TFD_NUM];
	pkt_t			*rx_ring_pkt[STAR_GSW_MAX_TFD_NUM];

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

	u8			phy_addr;
	u16			phy_id;

	u8			mac[6];
	port_config_t		port_config[3];
	vlan_config_t		vlan_config[8];
};

#endif

