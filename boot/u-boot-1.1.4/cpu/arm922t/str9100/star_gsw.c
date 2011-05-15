#include <common.h>

#ifdef CONFIG_DRIVER_STAR_GSW

#include <malloc.h>
#include <net.h>
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_gsw.h>
#include "star_gsw.h"
//#include "star_str9100_board.h"

//#define STAR_GSW_DEBUG

#define CPU_CACHE_BYTES		16
#define CPU_CACHE_ALIGN(X)	(((X) + (CPU_CACHE_BYTES-1)) & ~(CPU_CACHE_BYTES-1))

//#define CONFIG_VIRGO
//#define CONFIG_DORADO
//#define CONFIG_LIBRA
//#define CONFIG_LEO
//#define CONFIG_VELA

#ifdef CONFIG_VELA
#define	INIT_PORT0_PHY icp_101a_init(0);
#define	INIT_PORT1_PHY icp_101a_init(1);
#define PORT0_LINK_UP std_phy_power_down(0, 0);
#define PORT0_LINK_DOWN std_phy_power_down(0, 1);
#define PORT1_LINK_UP  std_phy_power_down(1,0);
#define PORT1_LINK_DOWN std_phy_power_down(1,1);
#endif


#ifdef CONFIG_VIRGO
#define	INIT_PORT0_PHY star_gsw_config_ASIX();
#define	INIT_PORT1_PHY star_gsw_config_AGERE();
#define PORT0_LINK_UP disable_AN(0, 1);
#define PORT0_LINK_DOWN disable_AN(0, 0);
#define PORT1_LINK_UP  std_phy_power_down(1,0);
#define PORT1_LINK_DOWN std_phy_power_down(1,1);
#endif

#ifdef CONFIG_DORADO
#define	INIT_PORT0_PHY star_gsw_config_port0_VSC7385();
#define	INIT_PORT1_PHY
#define PORT0_LINK_UP disable_AN(0, 1);
#define PORT0_LINK_DOWN disable_AN(0, 0);
#define PORT1_LINK_UP  
#define PORT1_LINK_DOWN 
#endif

#ifdef CONFIG_DORADO2
#define	INIT_PORT0_PHY star_gsw_config_port0_VSC7385();
//#define	INIT_PORT1_PHY star_gsw_config_VSC8201(1,1);
#define	INIT_PORT1_PHY star_gsw_config_VSC8X01();
#define PORT0_LINK_UP disable_AN(0, 1);
#define PORT0_LINK_DOWN disable_AN(0, 0);
#define PORT1_LINK_UP std_phy_power_down(1, 0);
#define PORT1_LINK_DOWN std_phy_power_down(1, 1);
#endif


#ifdef CONFIG_LIBRA
#define	INIT_PORT0_PHY star_gsw_config_icplus_175c_phy4(); 
#define	INIT_PORT1_PHY
#define PORT0_LINK_UP icp_175c_all_phy_power_down(0);
#define PORT0_LINK_DOWN icp_175c_all_phy_power_down(1);
#define PORT1_LINK_UP  
#define PORT1_LINK_DOWN 
#endif

#ifdef CONFIG_LEO
//#define	INIT_PORT0_PHY star_gsw_config_VSC8201(0,0);
#define	INIT_PORT0_PHY star_gsw_config_VSC8X01();
#define	INIT_PORT1_PHY
#define PORT0_LINK_UP std_phy_power_down(0,0);
#define PORT0_LINK_DOWN std_phy_power_down(0,1);
#define PORT1_LINK_UP  
#define PORT1_LINK_DOWN 
#endif


static struct star_gsw_dev_t star_gsw_dev;
static u8 star_gsw_dev_initialized;

static u8 default_mac_addr[] = {0x08, 0xaa, 0xbb, 0xcc, 0xdd, 0xee};

static TX_DESC_T	*tx_desc_pool;
static RX_DESC_T	*rx_desc_pool;
static pkt_t		*pkt_pool;
static u8		*pkt_buffer_pool;
static u8		mem_alloc_ok;

static int star_gsw_config_port0_VSC7385(void);
static void star_gsw_close(struct star_gsw_dev_t *dev);

int std_phy_power_down(int phy_addr, int y);
int disable_AN(int port, int y);

#if 1

#if 0
static void star_gsw_show_format_reg(u32 val)
{
	int i;

	for (i = 31; i >= 0; i--) {
		if (val & ((unsigned long)1 << i)) {
			printf("[%02d:1] ", i);
		} else {
			printf("[%02d:0] ", i);
		}
		if ((i % 8) == 0) {
			printf("\n");
		}
	}
	printf("==================================================================\n");
}

static void star_gsw_show_reg(void)
{
	return;
}
#endif

static int star_gsw_set_phy_addr(u8 mac_port, u8 phy_addr)
{
	if ((mac_port > 1) || (phy_addr > 31)) {
		return -1;
	}

	if (mac_port == 0) {
		GSW_PORT_MIRROR_REG &= ~(0x3 << 0); /* clear bit[1:0] for PHY_ADDR[1:0] */
		GSW_PORT_MIRROR_REG &= ~(0x3 << 4); /* clear bit[5:4] for PHY_ADDR[3:2] */
		GSW_QUEUE_STATUS_TEST_1_REG &= ~(0x1 << 25); /* clear bit[25] for PHY_ADDR[4] */
		GSW_PORT_MIRROR_REG |= (((phy_addr >> 0) & 0x3) << 0);
		GSW_PORT_MIRROR_REG |= (((phy_addr >> 2) & 0x3) << 4);
		GSW_QUEUE_STATUS_TEST_1_REG |= (((phy_addr >> 4) & 0x1) << 25);
	} else if (mac_port == 1) {
		GSW_PORT_MIRROR_REG &= ~(0x1 << 6); /* clear bit[6] for PHY_ADDR[0] */
		GSW_PORT_MIRROR_REG &= ~(0x7 << 8); /* clear bit[10:8] for PHY_ADDR[3:1] */
		GSW_QUEUE_STATUS_TEST_1_REG &= ~(0x1 << 26); /* clear bit[26] for PHY_ADDR[4] */
		GSW_PORT_MIRROR_REG |= (((phy_addr >> 0) & 0x1) << 6);
		GSW_PORT_MIRROR_REG |= (((phy_addr >> 1) & 0x7) << 8);
		GSW_QUEUE_STATUS_TEST_1_REG |= (((phy_addr >> 4) & 0x1) << 26);
	}

	return 0;
}

static int star_gsw_read_phy(u8 phy_addr, u8 phy_reg, u16 *read_data)
{
	u32 status;
	int i;

	// clear previous rw_ok status
	GSW_PHY_CONTROL_REG = (0x1 << 15);
	GSW_QUEUE_STATUS_TEST_1_REG &= ~(0xF << 16);
	GSW_PHY_CONTROL_REG &= ~(0x1 << 0);
	GSW_QUEUE_STATUS_TEST_1_REG = (((phy_addr >> 1) & 0xF) << 16);
	GSW_PHY_CONTROL_REG = ((phy_addr & 0x1) | ((phy_reg & 0x1F) << 8) | (0x1 << 14));

	for (i = 0; i < 0x1000; i++) {
		status = GSW_PHY_CONTROL_REG;
		if (status & (0x1 << 15)) {
			// clear the rw_ok status, and clear other bits value
			GSW_PHY_CONTROL_REG = (0x1 << 15);
			*read_data = (u16) ((status >> 16) & 0xFFFF);
			return (0);
		} else {
			udelay(100);
		}
	}

	return (-1);
}

static int star_gsw_write_phy(u8 phy_addr, u8 phy_reg, u16 write_data)
{
	int i;

	// clear previous rw_ok status
	GSW_PHY_CONTROL_REG = (0x1 << 15);
	GSW_QUEUE_STATUS_TEST_1_REG &= ~(0xF << 16);
	GSW_PHY_CONTROL_REG &= ~(0x1 << 0);
	GSW_QUEUE_STATUS_TEST_1_REG = (((phy_addr >> 1) & 0xF) << 16);
	GSW_PHY_CONTROL_REG = ((phy_addr & 0x1) |
		((phy_reg & 0x1F) << 8) |
		(0x1 << 13) | ((write_data & 0xFFFF) << 16));

	for (i = 0; i < 0x1000; i++) {
		if ((GSW_PHY_CONTROL_REG) & (0x1 << 15)) {
			// clear the rw_ok status, and clear other bits value
			GSW_PHY_CONTROL_REG = (0x1 << 15);
			return (0);
		} else {
			udelay(100);
		}
	}

	return (-1);
}
#endif

#ifdef CONFIG_VIRGO
static int star_gsw_config_AGERE(void)
{
        u32 mac_port_config;
        u16 phy_data = 0;
        int i;

        printf("configure port1 AGERE\n");

        /*
         * Configure MAC port 1
         * For Agere Systems's ET1011 single PHY
         */
        mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;

        // disable PHY's AN
        mac_port_config &= ~(0x1 << 7);

        // enable RGMII-PHY mode
        mac_port_config |= (0x1 << 15);

        GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;

        /*
         * configure Agere's ET1011 Single PHY
         */
        /* Configure Agere's ET1011 by Agere's programming note */
        //1. power-down the PHY
        star_gsw_read_phy(1, 0, &phy_data);
        phy_data |= (0x1 << 11);
        star_gsw_write_phy(1, 0, phy_data);


	//2. Enable PHY programming mode
	star_gsw_read_phy(1, 18, &phy_data);
	phy_data |= (0x1 << 0);
	phy_data |= (0x1 << 2);
	star_gsw_write_phy(1, 18, phy_data);

	//3.Perform some PHY register with the Agere-specfic value
	star_gsw_write_phy(1, 16, 0x880e);
	star_gsw_write_phy(1, 17, 0xb4d3);

	star_gsw_write_phy(1, 16, 0x880f);
	star_gsw_write_phy(1, 17, 0xb4d3);

	star_gsw_write_phy(1, 16, 0x8810);
	star_gsw_write_phy(1, 17, 0xb4d3);

	star_gsw_write_phy(1, 16, 0x8817);
	star_gsw_write_phy(1, 17, 0x1c00);

	star_gsw_write_phy(1, 16, 0x8805);
	star_gsw_write_phy(1, 17, 0xb03e);

	star_gsw_write_phy(1, 16, 0x8806);
	star_gsw_write_phy(1, 17, 0xb03e);

	star_gsw_write_phy(1, 16, 0x8807);
	star_gsw_write_phy(1, 17, 0xff00);

	star_gsw_write_phy(1, 16, 0x8808);
	star_gsw_write_phy(1, 17, 0xe110);

	star_gsw_write_phy(1, 16, 0x300d);
	star_gsw_write_phy(1, 17, 0x0001);

	//4. Disable PHY programming mode
	star_gsw_read_phy(1, 18, &phy_data);
	phy_data &= ~(0x1 << 0);
	phy_data &= ~(0x1 << 2);
	star_gsw_write_phy(1, 18, phy_data);

	//5. power-up the PHY
	star_gsw_read_phy(1, 0, &phy_data);
	phy_data &= ~(0x1 << 11);
	star_gsw_write_phy(1, 0, phy_data);

	star_gsw_read_phy(1, 22, &phy_data);

	// enable RGMII MAC interface mode : RGMII/RMII (dll delay or trace delay) mode
	phy_data &= ~(0x7 << 0);

	// phy_data |= (0x6 << 0); // RGMII/RMII dll delay mode : not work!!
	phy_data |= (0x4 << 0);	// RGMII/RMII trace delay mode

	star_gsw_write_phy(1, 22, phy_data);

	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;

	// enable PHY's AN
	mac_port_config |= (0x1 << 7);

	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;

	/*
	 * Enable flow-control on (Symmetric PAUSE frame)
	 */
	star_gsw_read_phy(1, 0x4, &phy_data);

	phy_data |= (0x1 << 10);

	star_gsw_write_phy(1, 0x4, phy_data);

	/*
	 * Enable PHY1 AN restart bit to restart PHY1 AN
	 */
	star_gsw_read_phy(1, 0x0, &phy_data);

	phy_data |= (0x1 << 9) | (0x1 << 12);

	star_gsw_write_phy(1, 0x0, phy_data);

	/*
	 * Polling until PHY1 AN restart is complete and PHY1 link status is UP
	 */
	for (i = 0; i < 0x2000; i++) {
		star_gsw_read_phy(1, 0x1, &phy_data);
		if ((phy_data & (0x1 << 5)) && (phy_data & (0x1 << 2))) {
			break;
		}
	}

	// adjust MAC port 1 RX/TX clock skew
	GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 28) | (0x3 << 30));
	GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 28) | (0x3 << 30));

	udelay(100);

	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	if (!(mac_port_config & 0x1) || (mac_port_config & 0x2)) {
		/*
		 * Port 1 PHY link down or no TXC in Port 1
		 */
		//PDEBUG("PHY1: Link Down, 0x%08x!\n", mac_port_config);
		return -1;
	}


	return 0;

}

static int star_gsw_config_ASIX(void)
{
	u32 mac_port_config;

	printf("configure port0 ASIX\n");
	mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;

	//Disable AN
	mac_port_config &= (~(0x1 << 7));

	//force speed to 1000Mbps
	mac_port_config &= (~(0x3 << 8));
	mac_port_config |= (0x2 << 8);	//jacky

	//force tx and rx follow control
	mac_port_config |= (0x1 << 11) | (0x1 << 12);

	//force full deplex
	mac_port_config |= 0x1 << 10;

	//RGMII ENABLR
	mac_port_config |= 0x1 << 15;

	GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;

	udelay(1000);

	/* adjust MAC port 0 RX/TX clock skew */
	GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 24) | (0x3 << 26));
	GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 24) | (0x2 << 26));
	
	// configure MAC port 0 pad drive strength = 10/100 mode
	*(u32 volatile *) (SYSPA_POWER_MANAGEMENT_BASE_ADDR + 0x1C) |= (0x1 << 2);
	return 0;
}
#endif

#ifdef CONFIG_LIBRA
void icp_175c_all_phy_power_down(int y)
{
        int i=0;

        for (i=0 ; i < 5 ; ++i)
                std_phy_power_down(i, y);

}

static int star_gsw_config_icplus_175c_phy4(void)
{
    u16			phy_data = 0, phy_data2 = 0;
    u32 volatile	ii, jj;
    u8			phy_speed_dup = 0, phy_flowctrl = 0;
    u32 volatile	reg;
	u8 gsw_mac_0_phy_addr = 0;
	u8 gsw_mac_1_phy_addr = 1;


	printf("config IC+175C\n");
    /*
     * Configure MAC port 0
     * For IP175C Switch setting
     * Force 100Mbps, and full-duplex, and flow control on
     */
    reg = GSW_MAC_PORT_0_CONFIG_REG;

    // disable PHY's AN
    reg &= ~(0x1 << 7);

    // disable RGMII-PHY mode
    reg &= ~(0x1 << 15);

    // force speed = 100Mbps
    reg &= ~(0x3 << 8);
    reg |= (0x1 << 8);
    
    // force full-duplex
    reg |= (0x1 << 10);

    // force Tx/Rx flow-control on
    reg |= (0x1 << 11) | (0x1 << 12);

    GSW_MAC_PORT_0_CONFIG_REG = reg;


    for (ii = 0; ii < 0x2000; ii++)
    {
    	reg = GSW_MAC_PORT_0_CONFIG_REG;
    	
        if ((reg & 0x1) && !(reg & 0x2))
        {
            /*
             * enable MAC port 0
             */
            reg &= ~(0x1 << 18);

           
            /*
             * enable the forwarding of unknown, multicast and broadcast packets to CPU
             */
            reg &= ~((0x1 << 25) | (0x1 << 26) | (0x1 << 27));
        
            /*
             * include unknown, multicast and broadcast packets into broadcast storm
             */
            reg |= ((0x1 << 29) | (0x1 << 30) | ((u32)0x1 << 31));
            
	    	GSW_MAC_PORT_0_CONFIG_REG = reg;
            
            break;
        }
        else
        {
            for (jj = 0; jj < 0x1000; jj++);
			
			
            if ((ii % 4) == 0)
                printf("\rCheck MAC/PHY 0 Link Status : |");
            else if ((ii % 4) == 1)
                printf("\rCheck MAC/PHY 0 Link Status : /");
            else if ((ii % 4) == 2)
                printf("\rCheck MAC/PHY 0 Link Status : -");
            else if ((ii % 4) == 3)
                printf("\rCheck MAC/PHY 0 Link Status : \\");
        }
    }


    if (!(reg & 0x1) || (reg & 0x2))
    {
        /*
         * Port 0 PHY link down or no TXC in Port 0
         */
        printf("\rCheck MAC/PHY 0 Link Status : DOWN!\n");
        
        return -1;
    }
    else
    {
        printf("\rCheck MAC/PHY 0 Link Status : UP!\n");
    }



    /*
     * Configure MAC port 1
     */
    	reg = GSW_MAC_PORT_0_CONFIG_REG;
    
    // disable MAC's AN
    reg &= ~(0x1 << 7);

    	GSW_MAC_PORT_0_CONFIG_REG = reg;


    /* enable flow control on (PAUSE frame) */
    star_gsw_read_phy(gsw_mac_1_phy_addr, 0x4, &phy_data);
		
    phy_data |= (0x1 << 10);  		

    star_gsw_write_phy(gsw_mac_1_phy_addr, 0x4, phy_data);

#if 1
	/* 2007/12/18 Jerry
		The software reset of IC+ 175C won't reset MII register 29, 30, 31.
		Router Control Register: bit 7 (TAG_VLAN_EN) is a VLAN related filed which affect vlan setting.
		Router Control Register: bit 3 (ROUTER_EN) enable router function at MII port.
		We set them to default to let U-boot properly work.
	*/
	phy_data = 0x1001;
    star_gsw_write_phy(30, 9, phy_data);
#endif
    /* restart PHY auto neg. */
    star_gsw_read_phy(gsw_mac_1_phy_addr, 0x0, &phy_data);
		
    phy_data |= (0x1 << 9) | (0x1 << 12);		

    star_gsw_write_phy(gsw_mac_1_phy_addr, 0x0, phy_data);



    /* wait for PHY auto neg. complete */
    for (ii = 0; ii < 0x20; ii++)
    {
        star_gsw_read_phy(gsw_mac_1_phy_addr, 0x1, &phy_data);
  			
        if ((phy_data & (0x1 << 2)) && (phy_data & (0x1 << 5)))
        {
            break;
        }
        else
        {			
            if ((ii % 4) == 0)
                printf("\rCheck MAC/PHY 1 Link Status : |");
            else if ((ii % 4) == 1)
                printf("\rCheck MAC/PHY 1 Link Status : /");
            else if ((ii % 4) == 2)
                printf("\rCheck MAC/PHY 1 Link Status : -");
            else if ((ii % 4) == 3)
                printf("\rCheck MAC/PHY 1 Link Status : \\");
        }
    }		


    if (ii >= 0x20)
    {
        printf("\rCheck MAC/PHY 1 Link Status : DOWN!\n");
        
        return -1;
    }
    else
    {
        printf("\rCheck MAC/PHY 1 Link Status : UP!\n");
    }


    star_gsw_read_phy(gsw_mac_1_phy_addr, 0x4, &phy_data);

    star_gsw_read_phy(gsw_mac_1_phy_addr, 0x5, &phy_data2);


    if (phy_data & 0x0400)	//FC on
    {
        //printf("<FC ON>");
        phy_flowctrl = 1;
    }
    else    
    {
        // printf("<FC OFF>");
        phy_flowctrl = 0;
    }    
    
    
    phy_speed_dup = 0;
    
    if ((phy_data & 0x0100) && (phy_data2 & 0x0100)) //100F
    {
        // printf("<100F>");
        phy_speed_dup |= (0x1 << 3); //set bit3 for 100F
    }
    else if ((phy_data & 0x0080) && (phy_data2 & 0x0080)) //100F
    {
        // printf("<100H>");
        phy_speed_dup |= (0x1 << 2);
    }
    else if ((phy_data & 0x0040) && (phy_data2 & 0x0040)) //100F
    {
        // printf("<10F>");
        phy_speed_dup |= (0x1 << 1);
    }
    else if ((phy_data & 0x0020) && (phy_data2 & 0x0020)) //100F
    {
        // printf("<10H>");
        phy_speed_dup |= 0x1;    
    }


    /*
     * Configure MAC port 1 in forced setting subject to the current PHY status
     */     
    	reg = GSW_MAC_PORT_1_CONFIG_REG;
     
    reg &= ~(0x1 << 7);	//AN off
            
    reg &= ~(0x3 << 8);
            
    if (phy_speed_dup & 0x0C)     //100
    {          
        //printf("<set 100>");
        reg |= (0x01 << 8);              
    }
    else if (phy_speed_dup & 0x03)     //10          
    {
        //printf("<set 10>");
        reg |= (0x00 << 8);
    }
           
    reg &= ~(0x1 << 11);
            
    if (phy_flowctrl)	//FC on
    {
        //printf("<set FC on>");
        reg |= (0x1 << 11);
    }	
    else
    {
        //printf("<set FC off>");
        reg |= (0x0 << 11);        	            	
    }            
            
    reg &= ~(0x1 << 10);
            
    if ((phy_speed_dup & 0x2) || (phy_speed_dup & 0x8))	//FullDup
    {
        //printf("<set full>");
        reg |= (0x1 << 10);
    }
    else	//HalfDup
    {
        //printf("<set half>");
        reg |= (0x0 << 10); //Half          	            	                
    }
            
    	GSW_MAC_PORT_1_CONFIG_REG = reg;


    /*
     * Check MAC port 1 link status
     */
    for (ii = 0; ii < 0x1000; ii++)
    {
    	reg = GSW_MAC_PORT_1_CONFIG_REG;
    	
        if ((reg & 0x1) && !(reg & 0x2))
        {
            /*
             * enable MAC port 1
             */
            reg &= ~(0x1 << 18);

            /*
             * enable the forwarding of unknown, multicast and broadcast packets to CPU
             */
            reg &= ~((0x1 << 25) | (0x1 << 26) | (0x1 << 27));
        
            /*
             * include unknown, multicast and broadcast packets into broadcast storm
             */
            reg |= ((0x1 << 29) | (0x1 << 30) | ((u32)0x1 << 31));
            
    	GSW_MAC_PORT_1_CONFIG_REG = reg;
            
            return 0;
        }
    }


    if (ii > 0x1000)
    {
        /*
         * Port 1 PHY link down or no TXC in Port 1
         */
        printf("\rCheck MAC/PHY 1 Link Status : DOWN!\n");
        
        return -1;
    }
	return 0;
}
#endif


static int star_gsw_config_VSC8201(u8 mac_port, u8 phy_addr)	// include cicada 8201
{
	//u32 mac_port_base = 0;
	u32 mac_port_config=0;
	u16 phy_reg;
	int i;

	printf("\nconfigure VSC8201\n");
	//PDEBUG("mac port : %d phy addr : %d\n", mac_port, phy_addr);
	/*
	 * Configure MAC port 0
	 * For Cicada CIS8201 single PHY
	 */
	if (mac_port == 0) {
		//PDEBUG("port 0\n");
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	}
	if (mac_port == 1) {
		//PDEBUG("port 1\n");
		mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	}

	star_gsw_set_phy_addr(mac_port, phy_addr);
	//star_gsw_set_phy_addr(1, 1);

	//mac_port_config = __REG(mac_port_base);

	// enable PHY's AN
	mac_port_config |= (0x1 << 7);

	// enable RGMII-PHY mode
	mac_port_config |= (0x1 << 15);

	// enable GSW MAC port 0
	mac_port_config &= ~(0x1 << 18);

	if (mac_port == 0) {
		//PDEBUG("port 0\n");
		GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;
	}
	if (mac_port == 1) {
		//PDEBUG("port 1\n");
		GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
	}

	/*
	 * Configure Cicada's CIS8201 single PHY
	 */
#ifdef CONFIG_STAR9100_SHNAT_PCI_FASTPATH
	/* near-end loopback mode */
	star_gsw_read_phy(phy_addr, 0x0, &phy_reg);
	phy_reg |= (0x1 << 14);
	star_gsw_write_phy(phy_addr, 0x0, phy_reg);
#endif

	star_gsw_read_phy(phy_addr, 0x1C, &phy_reg);

	// configure SMI registers have higher priority over MODE/FRC_DPLX, and ANEG_DIS pins
	phy_reg |= (0x1 << 2);

	star_gsw_write_phy(phy_addr, 0x1C, phy_reg);

	star_gsw_read_phy(phy_addr, 0x17, &phy_reg);

	// enable RGMII MAC interface mode
	phy_reg &= ~(0xF << 12);
	phy_reg |= (0x1 << 12);

	// enable RGMII I/O pins operating from 2.5V supply
	phy_reg &= ~(0x7 << 9);
	phy_reg |= (0x1 << 9);

	star_gsw_write_phy(phy_addr, 0x17, phy_reg);

	star_gsw_read_phy(phy_addr, 0x4, &phy_reg);

	// Enable symmetric Pause capable
	phy_reg |= (0x1 << 10);

	star_gsw_write_phy(phy_addr, 0x4, phy_reg);



	if (mac_port == 0) {
		//PDEBUG("port 0\n");
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	}
	if (mac_port == 1) {
		//PDEBUG("port 1\n");
		mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	}







	// enable PHY's AN
	mac_port_config |= (0x1 << 7);

	if (mac_port == 0) {
		//PDEBUG("port 0\n");
		GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;
	}
	if (mac_port == 1) {
		//PDEBUG("port 1\n");
		GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
	}

	/*
	 * Enable PHY1 AN restart bit to restart PHY1 AN
	 */
	star_gsw_read_phy(phy_addr, 0x0, &phy_reg);

	phy_reg |= (0x1 << 9) | (0x1 << 12);

	star_gsw_write_phy(phy_addr, 0x0, phy_reg);

	/*
	 * Polling until PHY0 AN restart is complete
	 */
	for (i = 0; i < 0x1000; i++) {
		star_gsw_read_phy(phy_addr, 0x1, &phy_reg);

		if ((phy_reg & (0x1 << 5)) && (phy_reg & (0x1 << 2))) {
			printf("0x1 phy reg: %x\n", phy_reg);
			break;
		} else {
			udelay(100);
		}
	}

	if (mac_port == 0) {
		//PDEBUG("port 0\n");
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	}
	if (mac_port == 1) {
		//PDEBUG("port 1\n");
		mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	}

	if (((mac_port_config & 0x1) == 0) || (mac_port_config & 0x2)) {
		printf("Check MAC/PHY%s Link Status : DOWN!\n", (mac_port == 0 ? "0" : "1"));
	} else {
		printf("Check MAC/PHY%s Link Status : UP!\n", (mac_port == 0 ? "0" : "1"));
		/*
		 * There is a bug for CIS8201 PHY operating at 10H mode, and we use the following
		 * code segment to work-around
		 */
		star_gsw_read_phy(phy_addr, 0x05, &phy_reg);

		if ((phy_reg & (0x1 << 5)) && (!(phy_reg & (0x1 << 6))) && (!(phy_reg & (0x1 << 7))) && (!(phy_reg & (0x1 << 8)))) {	/* 10H,10F/100F/100H off */
			star_gsw_read_phy(phy_addr, 0x0a, &phy_reg);

			if ((!(phy_reg & (0x1 << 10))) && (!(phy_reg & (0x1 << 11)))) {	/* 1000F/1000H off */
				star_gsw_read_phy(phy_addr, 0x16, &phy_reg);

				phy_reg |= (0x1 << 13) | (0x1 << 15);	// disable "Link integrity check(B13)" & "Echo mode(B15)"

				star_gsw_write_phy(phy_addr, 0x16, phy_reg);
			}
		}
	}

	if (mac_port == 0) {
		// adjust MAC port 0 RX/TX clock skew
		GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 24) | (0x3 << 26));
		GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 24) | (0x2 << 26));
	}

	if (mac_port == 1) {
		// adjust MAC port 1 RX/TX clock skew
		GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 28) | (0x3 << 30));
		GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 28) | (0x2 << 30));
	}

	return 0;
}


static int star_gsw_config_VSC8601(u8 mac_port, u8 phy_addr)
{
        u16 phy_data;
	u32 mac_port_config;
	
        printf("INIT VSC8601\n");
        if (mac_port == 0) {
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	}
	if (mac_port == 1) {
	        mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	}
		
        star_gsw_set_phy_addr(mac_port, phy_addr);

        // enable PHY's AN
	mac_port_config |= (0x1 << 7);
	
	// enable RGMII-PHY mode
	mac_port_config |= (0x1 << 15);
	
	// enable GSW MAC port 0
	mac_port_config &= ~(0x1 << 18);
        if (mac_port == 0) {
		GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;
	}
	if (mac_port == 1) {
		GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
	}
	
        star_gsw_read_phy(phy_addr, 3, &phy_data);
	if ((phy_data & 0x000f) == 0x0000) { // type A chip
                u16 tmp16;

                printf("VSC8601 Type A Chip\n");
                star_gsw_write_phy(phy_addr, 31, 0x52B5);
                star_gsw_write_phy(phy_addr, 16, 0xAF8A);

                phy_data = 0x0;
                star_gsw_read_phy(phy_addr, 18, &tmp16);
                phy_data |= (tmp16 & ~0x0);
                star_gsw_write_phy(phy_addr, 18, phy_data);

                phy_data = 0x0008;
                star_gsw_read_phy(phy_addr, 17, &tmp16);
                phy_data |= (tmp16 & ~0x000C);
                star_gsw_write_phy(phy_addr, 17, phy_data);

                star_gsw_write_phy(phy_addr, 16, 0x8F8A);

                star_gsw_write_phy(phy_addr, 16, 0xAF86);
                phy_data = 0x0008;
                star_gsw_read_phy(phy_addr, 18, &tmp16);
                phy_data |= (tmp16 & ~0x000C);
                star_gsw_write_phy(phy_addr, 18, phy_data);

                phy_data = 0x0;
                star_gsw_read_phy(phy_addr, 17, &tmp16);
                phy_data |= (tmp16 & ~0x0);
                star_gsw_write_phy(phy_addr, 17, phy_data);
                star_gsw_write_phy(phy_addr, 16, 0x8F8A);

                star_gsw_write_phy(phy_addr, 16, 0xAF82);

                phy_data = 0x0;
                star_gsw_read_phy(phy_addr, 18, &tmp16);
                phy_data |= (tmp16 & ~0x0);
                star_gsw_write_phy(phy_addr, 18, phy_data);

                phy_data = 0x0100;
                star_gsw_read_phy(phy_addr, 17, &tmp16);
                phy_data |= (tmp16 & ~0x0180);
                star_gsw_write_phy(phy_addr, 17, phy_data);

                star_gsw_write_phy(phy_addr, 16, 0x8F82);

                star_gsw_write_phy(phy_addr, 31, 0x0);
                //Set port type: single port
		star_gsw_read_phy(phy_addr, 9, &phy_data);
		phy_data &= ~( 0x1 << 10);
		star_gsw_write_phy(phy_addr, 9, phy_data);
        } else if ((phy_data & 0x000f) == 0x0001) { // type B chip
                printf("VSC8601 Type B Chip\n");
                star_gsw_read_phy(phy_addr, 23, &phy_data);
                phy_data |= ( 0x1 << 8); //set RGMII timing skew
                star_gsw_write_phy(phy_addr, 23, phy_data);
        }
    /*
     * Enable full-duplex mode
     */
    star_gsw_read_phy(phy_addr, 0, &phy_data);
    phy_data |= (0x1 << 8);
    star_gsw_write_phy(phy_addr, 0, phy_data);

        // change to extended registers
	star_gsw_write_phy(phy_addr, 31, 0x0001);
	
	star_gsw_read_phy(phy_addr, 28, &phy_data);
	phy_data &= ~(0x3 << 14); // set RGMII TX timing skew
//	phy_data |= (0x3 << 14); // 2.0ns
	phy_data &= ~(0x3 << 12); // set RGMII RX timing skew
//	phy_data |= (0x3 << 12); // 2.0ns
	star_gsw_write_phy(phy_addr, 28, phy_data);
	
	// change to normal registers
	star_gsw_write_phy(phy_addr, 31, 0x0000);
	
	if (mac_port == 0) {
		// adjust MAC port 0 RX/TX clock skew
		GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 24) | (0x3 << 26));
		GSW_BIST_RESULT_TEST_0_REG |= ((0x3 << 24) | (0x3 << 26));
	}

	if (mac_port == 1) {
		// adjust MAC port 1 RX/TX clock skew
		GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 28) | (0x3 << 30));
		GSW_BIST_RESULT_TEST_0_REG |= ((0x3 << 28) | (0x3 << 30));
	}
}


static void star_gsw_config_VSC8X01()
{
   u16	phy_id = 0;
   
#ifdef CONFIG_DORADO2
   star_gsw_set_phy_addr(1,1);
   star_gsw_read_phy(1, 0x02, &phy_id);
 //  printf("phy id = %X\n", phy_id);
   if (phy_id == 0x000F) //VSC8201
   	star_gsw_config_VSC8201(1,1);
   else
	star_gsw_config_VSC8601(1,1);
#else
#ifdef CONFIG_LEO
   star_gsw_set_phy_addr(0,0);
   star_gsw_read_phy(0, 0x02, &phy_id);
 //  printf("phy id = %X\n", phy_id);
   if (phy_id == 0x000F) //VSC8201
   	star_gsw_config_VSC8201(0,0);
   else
	star_gsw_config_VSC8601(0,0);
#endif
#endif
}

#ifdef CONFIG_VELA

#define PHY_CONTROL_REG_ADDR 0x00
#define PHY_AN_ADVERTISEMENT_REG_ADDR 0x04


int icp_101a_init (int port)
{
	u32 mac_port_config=0;
        u16 phy_data = 0;


	printf("init IC+101A\n");

	if (port == 0)		// port 0
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	if (port == 1)		// port 1
		mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;

	if (!(mac_port_config & (0x1 << 5))) {
		if (!star_gsw_read_phy (port, PHY_AN_ADVERTISEMENT_REG_ADDR, &phy_data))
	    	{
			//PDEBUG("\n PORT%d, enable local flow control capability Fail\n", port);
			return (1);
	    	}
		else
	    	{
	      		// enable PAUSE frame capability
			phy_data |= (0x1 << 10);

	      		if (!star_gsw_write_phy (port, PHY_AN_ADVERTISEMENT_REG_ADDR, phy_data))
			{
				//PDEBUG("\nPORT%d, enable PAUSE frame capability Fail\n", port);
				return (1);
			}
	    	}
	}


	// restart PHY0 AN
	if (!star_gsw_read_phy (port, PHY_CONTROL_REG_ADDR, &phy_data)) {
		//PDEBUG ("\n restart PHY%d AN Fail \n", port);
		return (1);
	}
	else {
		// enable PHY0 AN restart
		phy_data |= (0x1 << 9);

		if (!star_gsw_write_phy (port, PHY_CONTROL_REG_ADDR, phy_data)) {
			//PDEBUG ("\n  enable PHY0 AN restart \n");
			return (1);
		}
	}



	while (1)
	{
		//PDEBUG ("\n Polling  PHY%d AN \n", port);
		star_gsw_read_phy (port, PHY_CONTROL_REG_ADDR, &phy_data);

		if (phy_data & (0x1 << 9)) {
		  continue;
		}
		else {
			//PDEBUG ("\n PHY%d AN restart is complete \n", port);
			break;
		}
	}

	return 0;
}
#endif

#if defined(CONFIG_DORADO) || defined(CONFIG_DORADO2)
static int star_gsw_config_port0_VSC7385(void)
{
	u32 mac_port_config;
	int i;

	printf("config VSC7385\n");

	mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;

	// disable PHY's AN
	mac_port_config &= ~(0x1 << 7);

	// enable RGMII-PHY mode
	mac_port_config |= (0x1 << 15);

	// force speed = 1000Mbps
	mac_port_config &= ~(0x3 << 8);
	mac_port_config |= (0x2 << 8);

	// force full-duplex
	mac_port_config |= (0x1 << 10);

	// force Tx/Rx flow-control on
	mac_port_config |= (0x1 << 11) | (0x1 << 12);

	GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;

	udelay(1000);

	for (i = 0; i < 50000; i++) {
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
		if ((mac_port_config & 0x1) && !(mac_port_config & 0x2)) {
			break;
		} else {
			udelay(100);
		}
	}

	if (!(mac_port_config & 0x1) || (mac_port_config & 0x2)) {
		printf("MAC0 PHY Link Status : DOWN!\n");
		return -1;
	} else {
		printf("MAC0 PHY Link Status : UP!\n");
	}

	// enable MAC port 0
	mac_port_config &= ~(0x1 << 18);

	// disable SA learning
	mac_port_config |= (0x1 << 19);

	// forward unknown, multicast and broadcast packets to CPU
	mac_port_config &= ~((0x1 << 25) | (0x1 << 26) | (0x1 << 27));

	// storm rate control for unknown, multicast and broadcast packets
	mac_port_config |= (0x1 << 29) | (0x1 << 30) | ((u32)0x1 << 31);

	GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;

	// disable MAC port 1
	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	mac_port_config |= (0x1 << 18);
	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;

	// adjust MAC port 0 /RX/TX clock skew
	GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 24) | (0x3 << 26));
	GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 24) | (0x2 << 26));

	return 0;
}
#endif

static void star_gsw_get_mac_addr(struct star_gsw_dev_t *dev)
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

static int star_gsw_hw_init(struct star_gsw_dev_t *dev)
{
	u32 reg_config;
	int i, j;

	/* configure VLAN group */
	for (i = 0; i < 8; i++) {
  		dev->vlan_config[i].gid = i;
  		dev->vlan_config[i].vid = DEFAULT_VLAN_ID;
  		dev->vlan_config[i].port_map = 0;
  		dev->vlan_config[i].tag_port_map = 0;
	}

  	/* configure VLAN group 0, 1, 2 port map */
	dev->vlan_config[0].port_map = (GSW_VLAN_PORT_0 | GSW_VLAN_PORT_1 | GSW_VLAN_PORT_CPU);
	dev->vlan_config[1].port_map = (GSW_VLAN_PORT_0 | GSW_VLAN_PORT_CPU);
	dev->vlan_config[2].port_map = (GSW_VLAN_PORT_1 | GSW_VLAN_PORT_CPU);

	/* assign mac addr */
	star_gsw_get_mac_addr(dev);

	/* configure VLAN group mac addr */
	for (i = 0; i < 3; i++) {
		memcpy(dev->vlan_config[i].mac, dev->mac, 6);
	}

	/* configure MAC port 0 PVID */
	dev->port_config[0].pvid = dev->vlan_config[1].gid;
	dev->port_config[0].config = 0;
	dev->port_config[0].state = 0;

	/* configure MAC port 1 PVID */
	dev->port_config[1].pvid = dev->vlan_config[2].gid;
	dev->port_config[1].config = 0;
	dev->port_config[1].state = 0;

	/* configure CPU port PVID */
	dev->port_config[2].pvid = dev->vlan_config[0].gid;
	dev->port_config[2].config = 0;
	dev->port_config[2].state = 0;

	/* enable the gsw */
	HAL_PWRMGT_ENABLE_GSW();

	/* software reset the gsw */
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX);
	udelay(10);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX);
	udelay(10);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX);

	/* wait until all embedded memory BIST is complete */
	while (!(GSW_BIST_RESULT_TEST_0_REG & (0x1 << 17))) {
		udelay(100);
	}

	/* configure switch */
	reg_config = GSW_SWITCH_CONFIG_REG;

	/* disable aging */
	reg_config &= ~0xf;

	// max packet len 1536
	reg_config &= ~(3 << 4);
	reg_config |= (2 << 4);

	// crc strip
	reg_config |= (1 << 21);

	// disable IVL
	reg_config &= ~(1 << 22);

	// disable HNAT
	reg_config &= ~(1 << 23);

	GSW_SWITCH_CONFIG_REG = reg_config;
	
	/* configure cpu port */
	reg_config = GSW_CPU_PORT_CONFIG_REG;

	// enable CPU port
	reg_config &= ~(1 << 18);

	// disable CPU port SA learning
	reg_config |= (1 << 19);

	// DMA 4N mode
	reg_config |= ((u32)0x1 << 31);

	GSW_CPU_PORT_CONFIG_REG = reg_config;

	/* configure pvid */
	GSW_VLAN_PORT_PVID_REG =
		((dev->port_config[0].pvid & 0x7) << 0) |
		((dev->port_config[1].pvid & 0x7) << 4) |
		((dev->port_config[2].pvid & 0x7) << 8);

	/* configure vlan group id */
	GSW_VLAN_VID_0_1_REG =
		((dev->vlan_config[0].vid & 0xfff) << 0) |
		((dev->vlan_config[1].vid & 0xfff) << 12);

	GSW_VLAN_VID_2_3_REG =
		((dev->vlan_config[2].vid & 0xfff) << 0) |
		((dev->vlan_config[3].vid & 0xfff) << 12);
	
	GSW_VLAN_VID_4_5_REG =
		((dev->vlan_config[4].vid & 0xfff) << 0) |
		((dev->vlan_config[5].vid & 0xfff) << 12);
	
	GSW_VLAN_VID_6_7_REG =
		((dev->vlan_config[6].vid & 0xfff) << 0) |
		((dev->vlan_config[7].vid & 0xfff) << 12);
	
	// config VLAN port map
	reg_config = 0;
	for (i = 0, j = 0; i < 8; i++, j += 3) {
		reg_config |= ((dev->vlan_config[i].port_map & 0x7) << j);
	}
	GSW_VLAN_MEMBER_PORT_MAP_REG = reg_config;

	// config VLAN tag port map
	reg_config = 0;
	for (i = 0, j = 0; i < 8; i++, j += 3) {
		reg_config |= ((dev->vlan_config[i].tag_port_map & 0x7) << j);
	}
	GSW_VLAN_TAG_PORT_MAP_REG = reg_config;

	// config Default ARL Entry
	for (i = 0; i < 3; i++) {
		GSW_ARL_TABLE_ACCESS_CONTROL_0_REG = 0;
		GSW_ARL_TABLE_ACCESS_CONTROL_1_REG = 0;
		GSW_ARL_TABLE_ACCESS_CONTROL_0_REG = 0;

		GSW_ARL_TABLE_ACCESS_CONTROL_1_REG =
			((1 << 4) |
			 ((dev->vlan_config[i].gid & 0x7) << 5) |
			 (0x7 << 8) |
			 ((dev->vlan_config[i].port_map & 0x7) << 11) |
			 ((dev->vlan_config[i].mac[0] & 0xff) << 16) |
			 ((dev->vlan_config[i].mac[1] & 0xff) << 24));
	
		GSW_ARL_TABLE_ACCESS_CONTROL_2_REG =
			(((dev->vlan_config[i].mac[5] & 0xff) << 24) |
			 ((dev->vlan_config[i].mac[4] & 0xff) << 16) |
			 ((dev->vlan_config[i].mac[3] & 0xff) << 8)  |
			 ((dev->vlan_config[i].mac[2] & 0xff) << 0));

		/* write entry and wait */
		GSW_ARL_TABLE_ACCESS_CONTROL_0_REG = (1 << 3);

		for (j = 0; j < 100000; j++) {
			if (GSW_ARL_TABLE_ACCESS_CONTROL_1_REG & 0x1) {
				break;
			} else {
				udelay(1000);
			}
		}
		if (j == 100000) {
			return -1;
		}
	}

	// disable all interrupt status sources
	HAL_GSW_DISABLE_ALL_INTERRUPT_STATUS_SOURCES();

	// clear previous interrupt sources
	HAL_GSW_CLEAR_ALL_INTERRUPT_STATUS_SOURCES();

	// disable all DMA-related interrupt sources
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSQF_BIT_INDEX);

	// clear previous interrupt sources
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSQF_BIT_INDEX);

	/* stop all DMA operation */
	HAL_GSW_TS_DMA_STOP();
	HAL_GSW_FS_DMA_STOP();

	/* configure DMA descriptors */
	HAL_GSW_WRITE_TSSD((u32)dev->tx_desc_pool_dma);
	HAL_GSW_WRITE_TS_BASE((u32)dev->tx_desc_pool_dma);
	HAL_GSW_WRITE_FSSD((u32)dev->rx_desc_pool_dma);
	HAL_GSW_WRITE_FS_BASE((u32)dev->rx_desc_pool_dma);

	return 0;	
}

static inline pkt_t *star_gsw_alloc_pkt(struct star_gsw_dev_t *dev)
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

static inline void star_gsw_free_pkt(struct star_gsw_dev_t *dev, pkt_t *pkt)
{
	pkt->next = dev->free_pkt_list;
	dev->free_pkt_list = pkt;
	dev->free_pkt_count++;
}

static void star_gsw_tx_pkt_enqueue(struct star_gsw_dev_t *dev, pkt_t *pkt)
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

static pkt_t *star_gsw_tx_pkt_dequeue(struct star_gsw_dev_t *dev)
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

static void star_gsw_tx_pkt_requeue(struct star_gsw_dev_t *dev, pkt_t *pkt)
{
	pkt->next = dev->tx_pkt_q_head;
	dev->tx_pkt_q_head = pkt;
	if (dev->tx_pkt_q_tail == NULL) {
		dev->tx_pkt_q_tail = pkt;
	}
	dev->tx_pkt_q_count++;
}

static void star_gsw_free_mem(void)
{
	if (tx_desc_pool)
		free(tx_desc_pool);
	if (rx_desc_pool)
		free(rx_desc_pool);
	if (pkt_pool)
		free(pkt_pool);
	if (pkt_buffer_pool)
		free(pkt_buffer_pool);
}

static int star_gsw_alloc_mem(void)
{
	tx_desc_pool = (TX_DESC_T *)malloc(sizeof(TX_DESC_T) * STAR_GSW_MAX_TFD_NUM + CPU_CACHE_BYTES);
	if (tx_desc_pool == NULL) {
		goto err_out;
	}

	rx_desc_pool = (RX_DESC_T *)malloc(sizeof(TX_DESC_T) * STAR_GSW_MAX_RFD_NUM + CPU_CACHE_BYTES);
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
	star_gsw_free_mem();
	return -1;
}

static int star_gsw_init_mem(struct star_gsw_dev_t *dev)
{
	int i;

	dev->tx_desc_pool_dma = (TX_DESC_T *)CPU_CACHE_ALIGN((u32)tx_desc_pool);
	dev->rx_desc_pool_dma = (RX_DESC_T *)CPU_CACHE_ALIGN((u32)rx_desc_pool);
	memset(dev->tx_desc_pool_dma, 0, sizeof(TX_DESC_T) * STAR_GSW_MAX_TFD_NUM);
	memset(dev->rx_desc_pool_dma, 0, sizeof(RX_DESC_T) * STAR_GSW_MAX_RFD_NUM);
	dev->tx_desc_pool_dma[STAR_GSW_MAX_TFD_NUM - 1].end_of_descriptor = 1;
	dev->rx_desc_pool_dma[STAR_GSW_MAX_RFD_NUM - 1].end_of_descriptor = 1;

	memset(dev->tx_ring_pkt, 0, sizeof(pkt_t *) * STAR_GSW_MAX_TFD_NUM);
	memset(dev->rx_ring_pkt, 0, sizeof(pkt_t *) * STAR_GSW_MAX_RFD_NUM);

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

	for (i = 0; i < STAR_GSW_MAX_TFD_NUM; i++) {
		dev->tx_desc_pool_dma[i].cpu_own = 1;
		dev->tx_desc_pool_dma[i].ip_csum_offload = 0;
		/* the UDP packet issued by u-boot will not contain UDP checksum, do HW CSUM ? */
		/* it seems that if we enable udp csum, having chance to receive bad csum packet */
		dev->tx_desc_pool_dma[i].udp_csum_offload = 0;
		dev->tx_desc_pool_dma[i].tcp_csum_offload = 0;
	}
	
	for (i = 0; i < STAR_GSW_MAX_RFD_NUM; i++) {
		dev->rx_ring_pkt[i] = star_gsw_alloc_pkt(dev);
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

#ifdef STAR_GSW_DEBUG
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

static void star_gsw_halt(struct star_gsw_dev_t *dev)
{
	PORT0_LINK_DOWN
	PORT1_LINK_DOWN
#if 0
	// disable all interrupt status sources
	HAL_GSW_DISABLE_ALL_INTERRUPT_STATUS_SOURCES();

	// clear previous interrupt sources
	HAL_GSW_CLEAR_ALL_INTERRUPT_STATUS_SOURCES();

	// disable all DMA-related interrupt sources
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSQF_BIT_INDEX);

	// clear previous interrupt sources
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSQF_BIT_INDEX);

	// stop all DMA operation
	HAL_GSW_TS_DMA_STOP();
	HAL_GSW_FS_DMA_STOP();

	// disable CPU port, GSW MAC port 0 and MAC port 1
	GSW_MAC_PORT_0_CONFIG_REG |= (0x1 << 18);
	GSW_MAC_PORT_1_CONFIG_REG |= (0x1 << 18);
	GSW_CPU_PORT_CONFIG_REG |= (0x1 << 18);

	// software reset the gsw
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX);
	udelay(10);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX);
	udelay(10);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX);

	/* wait until all embedded memory BIST is complete */
	while (!(GSW_BIST_RESULT_TEST_0_REG & (0x1 << 17))) {
		udelay(100);
	}
#endif
}

static int star_gsw_init(struct star_gsw_dev_t *dev)
{
	int err;

	if (!mem_alloc_ok) {
		err = star_gsw_alloc_mem();
		if (err) {
			return err;
		}
	}

	star_gsw_init_mem(dev);

	err = star_gsw_hw_init(dev);
	if (err) {
		star_gsw_halt(dev);
		memset(dev, 0, sizeof(struct star_gsw_dev_t));
		return err;
	}

	return 0;
}




int std_phy_power_down(int phy_addr, int y)
{
        u16 phy_data = 0;
        /* set isolate bit instead of powerdown */
        star_gsw_read_phy(phy_addr, 0, &phy_data);
        if (y==1) // set isolate
                phy_data |= (0x1 << 10);
        if (y==0) // unset isolate
                phy_data &= (~(0x1 << 10));
        star_gsw_write_phy(phy_addr, 0, phy_data);
        return 0;
}


// port : 0 => port0 ; port : 1 => port1
// y = 1 ; disable AN
int disable_AN(int port, int y)
{
        u32 mac_port_config=0;

        if (port==0)
                mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
        if (port==1)
                mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
        // disable PHY's AN
        if (y==1)
        {
          //PDEBUG("disable AN\n");
          mac_port_config &= ~(0x1 << 7);
        }

        // enable PHY's AN
        if (y==0)
        {
          //PDEBUG("enable AN\n");
          mac_port_config |= (0x1 << 7);
        }

        if (port==0)
                GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;
        if (port==1)
                GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
        return 0;
}



static void star_gsw_open(struct star_gsw_dev_t *dev)
{
	//static int init_phy=0;

	// config the phy
	INIT_PORT0_PHY 
	INIT_PORT1_PHY


	GSW_MAC_PORT_0_CONFIG_REG &= ~(0x1 << 18);
	GSW_MAC_PORT_1_CONFIG_REG &= ~(0x1 << 18);
	GSW_CPU_PORT_CONFIG_REG &= ~(0x1 << 18);

	// start rx DMA operation
	HAL_GSW_FS_DMA_START();

	PORT0_LINK_UP
	PORT1_LINK_UP
}

static void star_gsw_close(struct star_gsw_dev_t *dev)
{
	star_gsw_halt(dev);
}

static void star_gsw_rx(struct star_gsw_dev_t *dev)
{
	RX_DESC_T volatile *rx_desc;
	pkt_t *rcvpkt;
	pkt_t *newpkt;
	u32 rxcount = 0;
	while (1) {
		rx_desc = &dev->rx_desc_pool_dma[dev->cur_rx_desc_idx];
		if (rx_desc->cpu_own == 0) {
			break;
		}
		rcvpkt = dev->rx_ring_pkt[dev->cur_rx_desc_idx];
		rcvpkt->length = rx_desc->length;
		newpkt = star_gsw_alloc_pkt(dev);
		if (newpkt == NULL) {
			printf("Allocate pkt failed on RX...\n");
		}
		dev->rx_ring_pkt[dev->cur_rx_desc_idx] = newpkt;
		rx_desc->data_ptr = (u32)newpkt->pkt_buffer;
		rx_desc->length = PKT_BUFFER_SIZE;
		rx_desc->cpu_own = 0;
#ifdef STAR_GSW_DEBUG
		printf("RX PKT buffer: 0x%08x\n", rcvpkt->pkt_buffer);
		printf("RX PKT length: %d\n", rcvpkt->length);
#endif
		NetReceive(rcvpkt->pkt_buffer, rcvpkt->length);
		star_gsw_free_pkt(dev, rcvpkt);

		if (dev->cur_rx_desc_idx == (STAR_GSW_MAX_RFD_NUM - 1)) {
			dev->cur_rx_desc_idx = 0;
		} else {
			dev->cur_rx_desc_idx++;
		}

		rxcount++;
		if (rxcount == STAR_GSW_MAX_RFD_NUM) {
			break;
		}
	}

	dev->rx_pkt_count += rxcount;
}

static void star_gsw_tx(struct star_gsw_dev_t *dev)
{
	TX_DESC_T volatile *tx_desc;
	pkt_t *pkt;
	u32 txcount = 0;

	while ((pkt = star_gsw_tx_pkt_dequeue(dev))) {
		tx_desc = &dev->tx_desc_pool_dma[dev->cur_tx_desc_idx];
		if (!tx_desc->cpu_own) {
			star_gsw_tx_pkt_requeue(dev, pkt);
			break;
		} else {
			if (dev->tx_ring_pkt[dev->cur_tx_desc_idx]) {
				star_gsw_free_pkt(dev, dev->tx_ring_pkt[dev->cur_tx_desc_idx]);
			}
		}
#ifdef STAR_GSW_DEBUG
		printf("TX PKT buffer: 0x%08x\n", pkt->pkt_buffer);
		printf("TX PKT length: %d\n", pkt->length);
#endif
		dev->tx_ring_pkt[dev->cur_tx_desc_idx] = pkt;
		tx_desc->data_ptr = (u32)pkt->pkt_buffer;
		tx_desc->length = pkt->length;
		tx_desc->cpu_own = 0;
		tx_desc->first_segment = 1;
		tx_desc->last_segment = 1;

		if (dev->cur_tx_desc_idx == (STAR_GSW_MAX_TFD_NUM - 1)) {
			dev->cur_tx_desc_idx = 0;
		} else {
			dev->cur_tx_desc_idx++;
		}

		txcount++;
	}

	dev->tx_pkt_count += txcount;

	HAL_GSW_TS_DMA_START();
}

int eth_init(bd_t *bis)
{
	struct star_gsw_dev_t *dev = &star_gsw_dev;
	int err = 0;

#ifdef STAR_GSW_DEBUG
	printf("eth_init()\n");
#endif

	if (star_gsw_dev_initialized) {
		return 0;
	}

	if (star_gsw_init(dev) == 0) {
		star_gsw_open(dev);
		star_gsw_dev_initialized = 1;
	} else {
		err = -1;
	}

	return err;
}

s32 eth_send(volatile void *packet, s32 length)
{
	struct star_gsw_dev_t *dev = &star_gsw_dev;

	pkt_t *pkt;

	pkt = star_gsw_alloc_pkt(dev);
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
	star_gsw_tx_pkt_enqueue(dev, pkt);
	star_gsw_tx(dev);

	return 0;
}

s32 eth_rx(void)
{
	struct star_gsw_dev_t *dev = &star_gsw_dev;

	star_gsw_rx(dev);

	return 0;
}

void eth_halt(void)
{
	struct star_gsw_dev_t *dev = &star_gsw_dev;

#ifdef STAR_GSW_DEBUG
	printf("eth_halt()\n");
#endif

	if (star_gsw_dev_initialized) {
		star_gsw_close(dev);
		//memset(&star_gsw_dev, 0, sizeof(struct star_gsw_dev_t));
		star_gsw_dev_initialized = 0;
	}
}

#endif

