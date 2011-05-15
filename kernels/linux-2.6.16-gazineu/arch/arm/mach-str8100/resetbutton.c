/******************************************************************************
 *  button_wlx652.c: Monitoring of reset button WLX 652 board.
 *
 *  If the reset is pressed on module startup, and keep pressed for 6 seconds,
 *  this module will restore Snake Os default config
 *  If the button is released before, the module loads and wait for a button 
 *  push, and reboot the NAS
 * 
 *  It create an interupt for the button that is connected to the following pin:
 *     GPIOA[13] = Reset Button
 *
 *  Created by Douglas Gazineu
 * ******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 * 
 *  This file is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License, Version 2, as 
 *  published by the Free Software Foundation. 
 *
 *  This file is distributed in the hope that it will be useful, 
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or 
 *  NONINFRINGEMENT.  See the GNU General Public License for more details. 
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this file; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or 
 *  visit http://www.gnu.org/licenses/. 
 *
 *  This file may also be available under a different license from Cavium. 
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp_lock.h>
#include <linux/delay.h>

#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/hardware.h>
#include <asm/mach-types.h>

#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_gpio.h>

#define	RESET_BUTTON GPIO_13_MASK

//#include <drivers/star/str8100/str8100_led.h>
// extern void str8100_led_all_on(void);
// extern void str8100_led_all_off(void);
// extern void str8100_led_on(unsigned int led_index);
// extern void str8100_led_off(unsigned int led_index);
// extern void str8100_led_toggle(unsigned int led_index);
// extern void str8100_led_init(void);

extern void str8100_set_interrupt_trigger(unsigned int, unsigned int, unsigned int, unsigned int);
static struct work_struct reboot_work;
void do_reboot(){

	char* argv[2];
	char* env[1];
	u32 retval;

	argv[0]="/sbin/reboot";
	argv[1]=NULL;
	env[0]=NULL;

	retval=call_usermodehelper(argv[0],argv,env,0);
	printk("Rebooting...(retval=%d(0x%x)\n",retval,retval);
	msleep(5000);
}

static irqreturn_t str8100_gpio_irq_handler(int this_irq, void *dev_id, struct pt_regs *regs)
{
    unsigned int volatile status;

    HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
    HAL_GPIOA_READ_INTERRUPT_MASKED_STATUS(status);

	INIT_WORK(&reboot_work,do_reboot,NULL);
	schedule_work(&reboot_work);

	HAL_GPIOA_CLEAR_INTERRUPT(status);

	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_GPIO_EXTERNAL_INT_BIT_INDEX);

    return IRQ_HANDLED;
}

static int __init reset_handler(void){
	int ret;
	__u32 data;
	int cnt;
	cnt = 0;

	char* argv[3];
	char* env[2];
	u32 retval;

	argv[0]="/usr/share/snake/config";
	argv[1]="setdefault";
	argv[2]=NULL;
	env[0]="PATH=/sbin:/usr/sbin:/bin:/usr/bin";
	env[1]=NULL;


    HAL_PWRMGT_ENABLE_GPIO_CLOCK();

    PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_GPIO_SOFTWARE_RESET_BIT_INDEX);

	printk("Reset config: ");

	HAL_GPIOA_SET_DIRECTION_INPUT(RESET_BUTTON);
	HAL_GPIOA_ENABLE_INTERRUPT(RESET_BUTTON);
	HAL_GPIOA_DISABLE_INTERRUPT_MASK(RESET_BUTTON);
    HAL_GPIOA_SET_INTERRUPT_LEVEL_TRIGGER_MODE(RESET_BUTTON);
	HAL_GPIOA_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(RESET_BUTTON);

	while(cnt<6){
		HAL_GPIOA_READ_DATA_IN_STATUS(data);

		if ( ! (data & RESET_BUTTON) ){
			printk(".");
			if ( cnt == 5 ){
				printk(" YES\nRestoring default SNAKE OS config...\n");
				retval=call_usermodehelper(argv[0],argv,env,0);
				msleep(10000);
			}
			msleep(1000);
		}
		else {
			cnt = 10;
			printk("NO\n");
		}
		cnt++;
	}


	HAL_GPIOA_CLEAR_INTERRUPT(RESET_BUTTON);
	HAL_GPIOA_SET_DIRECTION_INPUT(RESET_BUTTON);
	HAL_GPIOA_ENABLE_INTERRUPT(RESET_BUTTON);
	HAL_GPIOA_DISABLE_INTERRUPT_MASK(RESET_BUTTON);
    HAL_GPIOA_SET_INTERRUPT_LEVEL_TRIGGER_MODE(RESET_BUTTON);
	HAL_GPIOA_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(RESET_BUTTON);

    str8100_set_interrupt_trigger (INTC_GPIO_EXTERNAL_INT_BIT_INDEX,INTC_IRQ_INTERRUPT,INTC_LEVEL_TRIGGER,INTC_ACTIVE_HIGH);

	if ((ret=request_irq(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, str8100_gpio_irq_handler, 0, "testing", NULL))){
		printk("%s: request_irq failed(ret=0x%x)(-EBUSY=0x%x)\n",__FUNCTION__,ret,-EBUSY);
		return -EBUSY;
	}

	return 0;
}

static void __exit reset_handler_exit(void){
	printk("%s: \n",__FUNCTION__);
	free_irq(INTC_GPIO_EXTERNAL_INT_BIT_INDEX,NULL);
}

module_init(reset_handler);
module_exit(reset_handler_exit);
