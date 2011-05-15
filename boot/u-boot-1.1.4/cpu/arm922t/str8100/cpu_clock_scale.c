#include <common.h>
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_timer.h>
#include <asm/arch/star_intc.h>

#ifdef CONFIG_LOW_FREQ
	#include <asm/arch/star_wdtimer.h>
        #include <asm/arch/star_misc.h>	
#endif

#define TIMER_COUNTER_VAL	100

static inline str8100_cpu_enter_idle_mode(void)
{
	u32 zero = 0;
	__asm__ __volatile__("mcr	p15, 0, %0, c7, c0, 4" : : "r" (zero) : "cc");
}

static void str8100_adjust_dram_auto_refresh_interval(u32 old_ahb_clock, u32 new_ahb_clock)
{
	u32 dramc_timing_parameter2;
	u32 old_auto_refresh_interval;
	u32 new_auto_refresh_interval;

	dramc_timing_parameter2 = (*((u32 volatile *)(SYSPA_DDRC_SDRC_BASE_ADDR + 0x18)));
	old_auto_refresh_interval = dramc_timing_parameter2 & 0xFFF;
	new_auto_refresh_interval = ((old_auto_refresh_interval + 1) * new_ahb_clock) / (old_ahb_clock) - 1;
	dramc_timing_parameter2 &= ~0xFFF;
	dramc_timing_parameter2 |= new_auto_refresh_interval & 0xFFF;
	(*((u32 volatile *)(SYSPA_DDRC_SDRC_BASE_ADDR + 0x18))) = dramc_timing_parameter2;
}

static u32 str8100_ahb_clock(void)
{
	u32 PLL_clock;
	u32 CPU_clock;
	u32 AHB_clock;

	switch (PWRMGT_SYSTEM_CLOCK_CONTROL_REG & 0x3) {
	case 0x0:
		PLL_clock = 175000000;
		break;
	case 0x1:
		PLL_clock = 200000000;
		break;
	case 0x2:
		PLL_clock = 225000000;
		break;
	case 0x3:
		PLL_clock = 250000000;
		break;
	}

	CPU_clock = PLL_clock / (((PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 2) & 0x3) + 1);
	AHB_clock = CPU_clock / (((PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 4) & 0x3) + 1);

	return AHB_clock / 1000000;
}

void str8100_cpu_clock_show(void)
{
	u32 PLL_clock;
	u32 CPU_clock;
	u32 AHB_clock;
	u32 APB_clock;

	switch (PWRMGT_SYSTEM_CLOCK_CONTROL_REG & 0x3) {
	case 0x0:
		PLL_clock = 175000000;
		break;
	case 0x1:
		PLL_clock = 200000000;
		break;
	case 0x2:
		PLL_clock = 225000000;
		break;
	case 0x3:
		PLL_clock = 250000000;
		break;
	}

	CPU_clock = PLL_clock / (((PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 2) & 0x3) + 1);
	AHB_clock = CPU_clock / (((PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 4) & 0x3) + 1);
	APB_clock = AHB_clock / (((PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 8) & 0x3) + 1);

	printf("PLL clock at %dMHz\n", PLL_clock / 1000000);
	printf("CPU clock at %dMHz\n", CPU_clock / 1000000);
	printf("AHB clock at %dMHz\n", AHB_clock / 1000000);
	printf("APB clock at %dMHz\n", APB_clock / 1000000);
}

void str8100_cpu_clock_scale_end(void)
{
	u32 status;

	// disable timer2
	HAL_TIMER_DISABLE_TIMER2();
	// clear timer2 interrupt status
	status = TIMER1_TIMER2_INTERRUPT_STATUS_REG;
	TIMER1_TIMER2_INTERRUPT_STATUS_REG = status;
	TIMER1_TIMER2_INTERRUPT_MASK_REG = 0x3f;

	status = INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG;
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG = status;
	INTC_INTERRUPT_MASK_REG = 0xFFFFFFFF;
}

#ifdef CONFIG_LOW_FREQ
static void str8100_enable_wdt(void)
{
    
    HAL_PWRMGT_ENABLE_WDTIMER_CLOCK();

    //Hal_Pwrmgt_Software_Reset(PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX);
     {
     // set high    
     PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX );
     
     // set low
     PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX);
     
     // set high
     PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX );
     }


    // Disable WDTimer
    WDTIMER_CONTROL_REG &= ~(WDTIMER_ENABLE_BIT);

    // set up clock source
    HAL_WDTIMER_CLOCK_SOURCE_EXTCLK();
    // enable system reset of wdt
    HAL_WDTIMER_ENABLE_SYSTEM_RESET();
    

    // set up wdt counter
    HAL_WDTIMER_WRITE_AUTO_RELOAD_COUNTER(100);
    
    //  wdt restart
    HAL_WDTIMER_ENABLE_RESTART_RELOAD();

    // enable wdt
    HAL_WDTIMER_ENABLE();
    
}
#endif /* CONFIG_LOW_FREQ  */

void str8100_cpu_clock_scale_start(void)
{
	u32 timer_control;
	u32 status;
	u32 last_time;
	u32 env_pll_clock = 175;
	u32 env_pll_to_cpu_ratio = 1;
	u32 env_cpu_to_ahb_ratio = 2;
	u32 new_pll_clock;
	u32 new_cpu_clock;
	u32 new_ahb_clock;
	u32 old_ahb_clock;
	char *s;

#ifdef CONFIG_LOW_FREQ
	unsigned long reboot_idx = 0;
	char rbt_idx_set[10];
	char *rbt_idx;
#endif /* CONFIG_LOW_FREQ */

	old_ahb_clock = str8100_ahb_clock();

	if ((s = getenv("cpu_clock")) != NULL) {
		env_pll_clock = simple_strtoul(s, NULL, 10);
	} else {
		if ((s = getenv("pll_clock")) != NULL) {
			env_pll_clock = simple_strtoul(s, NULL, 10);
		}

		if ((s = getenv("pll_to_cpu_ratio")) != NULL) {
			env_pll_to_cpu_ratio = simple_strtoul(s, NULL, 10);
		}

		if ((s = getenv("cpu_to_ahb_ratio")) != NULL) {
			env_cpu_to_ahb_ratio = simple_strtoul(s, NULL, 10);
		}
	}
	
  	#ifdef CONFIG_LOW_FREQ
	// enable wdt , and start to count down
    str8100_enable_wdt();
           
    // check if wdt system reboot or not ?	
    //
	rbt_idx = getenv ("reboot_idx");
	reboot_idx = rbt_idx ? (int)simple_strtol(rbt_idx, NULL, 10) : CONFIG_REBOOTIDX;
   
    if ( reboot_idx != 0 )
	{  // it means system is reset by wdt , we low down CPU from 250MHZ to 200MHZ	
	   env_pll_clock = 200;
	   
	   // reset reboot_idx , next time CPU clock will be rechecked again
       reboot_idx = 0;
       sprintf (rbt_idx_set, "%lu", reboot_idx);
	   setenv ("reboot_idx", rbt_idx_set);
	    
   
	}
	else
	{  // it is the first time boot, we set CPU clock to 250MHZ as default
	   reboot_idx = 1;
	   sprintf (rbt_idx_set, "%lu", reboot_idx);
	   setenv ("reboot_idx", rbt_idx_set);
     	
	}
        
    // save environment
    saveenv();
	#endif /* CONFIG_LOW_FREQ */


	switch(env_pll_clock) {
	case 175:
		HAL_PWRMGT_SET_PLL_FREQUENCY_175MHZ();
		new_pll_clock = 175;
		break;
	case 200:
		HAL_PWRMGT_SET_PLL_FREQUENCY_200MHZ();
		new_pll_clock = 200;
		break;
	case 225:
		HAL_PWRMGT_SET_PLL_FREQUENCY_225MHZ();
		new_pll_clock = 225;
		break;
	case 250:
		HAL_PWRMGT_SET_PLL_FREQUENCY_250MHZ();
		new_pll_clock = 250;
		break;
	default:
		return;
		break;
	}

	switch(env_pll_to_cpu_ratio) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		env_pll_to_cpu_ratio = 1;
		break;
	}
	new_cpu_clock = new_pll_clock / env_pll_to_cpu_ratio;

	switch (env_cpu_to_ahb_ratio) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		env_cpu_to_ahb_ratio = 2;
	}
	new_ahb_clock = new_cpu_clock / env_cpu_to_ahb_ratio;
	if (new_ahb_clock > 125) {
		env_cpu_to_ahb_ratio++;
		new_ahb_clock = new_cpu_clock / env_cpu_to_ahb_ratio;
	}

	HAL_PWRMGT_CONFIG_PLLCLK_TO_CPUCLK_RATIO(env_pll_to_cpu_ratio);
	HAL_PWRMGT_CONFIG_CPUCLK_TO_HCLK_RATIO(env_cpu_to_ahb_ratio);

	INTC_INTERRUPT_MASK_REG = 0xFFFFFFFF;
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG = 0xFFFFFFFF;
	INTC_SOFTWARE_INTERRUPT_CLEAR_REG = 0xFFFFFFFF;
	INTC_SOFTWARE_PRIORITY_MASK_REG = 0x0;
	INTC_FIQ_SELECT_REG = 0x0;
        INTC_VECTOR_INTERRUPT_ENABLE_REG = 0;

	HAL_TIMER_DISABLE_TIMER2();

	TIMER2_COUNTER_REG = TIMER_COUNTER_VAL;
	TIMER2_AUTO_RELOAD_VALUE_REG = TIMER_COUNTER_VAL;
	TIMER2_MATCH_VALUE1_REG = 0;
	TIMER2_MATCH_VALUE2_REG = 0;

	// mask all the timer interrupt
	TIMER1_TIMER2_INTERRUPT_MASK_REG = 0x3f;

	timer_control = TIMER1_TIMER2_CONTROL_REG;

	// timer2 down counter
	timer_control |= (1 << TIMER2_UP_DOWN_COUNT_BIT_INDEX);

	// timer2 enable overflow interrupt
	timer_control |= (1 << TIMER2_OVERFLOW_INTERRUPT_BIT_INDEX);

	/* timer2 seleck 1KHz Clock */
	timer_control |= (1 << TIMER2_CLOCK_SOURCE_BIT_INDEX);

	/* timer2 enable */
	timer_control |= (1 << TIMER2_ENABLE_BIT_INDEX);

	TIMER1_TIMER2_CONTROL_REG = timer_control;

	// unmask timer2 overflow interrupt
	TIMER1_TIMER2_INTERRUPT_MASK_REG = 0x1f;

	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_TIMER2_BIT_INDEX);

	HAL_PWRMGT_DISABLE_DRAMC_CLOCK();
	if (new_ahb_clock < old_ahb_clock) {
		str8100_adjust_dram_auto_refresh_interval(old_ahb_clock, new_ahb_clock);
	}

	str8100_cpu_enter_idle_mode();
}

