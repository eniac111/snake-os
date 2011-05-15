/*
 *  driverled_nas652.c: Creates a char device that can turn the on and off the
 *  leds of the NAS 652 board.
 *   
 *  It control the leds that are connected to the following pins:
 *     GPIOA[1] = system led
 *     GPIOA[16] = USB port 1 led
 *     GPIOA[17] = USB port 2 led 
 *
 * 
 * To control the leds, write a unsigned char (1 byte) to the corresponding 
 * device file in /dev.
 * 
 * The byte that control the leds has the following meaning:
 *
 * Action: 4 most significant bits:
 *  0010 = (0x2) togle
 *  0011 = (0x3) turn all leds off (does not matter the 4 less significant bits)
 *  0100 = (0x4) turn all leds on (does not matter the 4 less significant bits)
 *  0101 = (0x5) turn a led on
 *  0110 = (0x6) turn a led off 
 *
 * Led number: 4 less significant bits:
 *  0001 = (0x1) led 1
 *  0010 = (0x2) led 2
 *  0011 = (0x3) led 3
 *
 *  Crated by Miguel Moreto
 *
 *******************************************************************************
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
 *****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/types.h>
#include <asm/uaccess.h>        /* for put_user */
#include <linux/delay.h>
#include <asm/arch/star_gpio.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>

MODULE_LICENSE("GPL");

/*  
 *  Prototypes - this would normally go in a .h file
 */
//int init_module(void);
//void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "driverled652"   /* Dev name as it appears in /proc/devices   */
#define DRIVER_AUTHOR "Miguel Moreto <miguelmoreto@gmail.com>"
#define DRIVER_DESC "Driver interface for NAS 652 leds."

#define BUF_LEN 80              /* Max length of the message from the device */

#define LED_MASK_A 			0xfe3fe07f

#define LED_MASK 			LED_MASK_A
#define GPIO_DIRECTION_REG		GPIOA_DIRECTION_REG
#define GPIO_DATA_OUTPUT_REG 		GPIOA_DATA_OUTPUT_REG
#define GPIO_DATA_BIT_SET_REG 		GPIOA_DATA_BIT_SET_REG
#define GPIO_DATA_BIT_CLEAR_REG 	GPIOA_DATA_BIT_CLEAR_REG

#define LED_DELAY_MS		500



/* 
 * Global variables are declared as static, so are global within the file. 
 */

static int Major;               /* Major number assigned to our device driver */
static int Device_Open = 0;     /* Is device open?  
                                 * Used to prevent multiple access to device */
static char msg[BUF_LEN];       /* The msg the device will give when asked */
static char *msg_Ptr;

//static struct class *driverled_class;

static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};
 
/*
 * ===============================================================================
 *  LED CONTROL FUNCTIONS:
 *  Copyright (c) 2008 Cavium Networks 
 * ===============================================================================
 */
/*
 * Configure all LEDs on
 */
void str8100_led_all_on(void)
{
    /*
     * perform Write Low to GPIO Pin
     */    
    GPIO_DATA_BIT_CLEAR_REG |= LED_MASK;
}

/*
 * Configure all LEDs off
 */
void str8100_led_all_off(void)
{
    /*
     * perform Write High to GPIO Pin
     */
    GPIO_DATA_BIT_SET_REG |= LED_MASK;
}

/*
 * Configure one LED on
 */
void str8100_led_on(unsigned int led_index)
{
    /*
     * perform Write Low to GPIO Pin
     */
    GPIO_DATA_BIT_CLEAR_REG |= (led_index & LED_MASK);
}

/*
 * Configure one LED off
 */
void str8100_led_off(unsigned int led_index)
{
    /*
     * perform Write High to GPIO Pin
     */
    GPIO_DATA_BIT_SET_REG |= (led_index & LED_MASK);
}

/*
 * Toggle one LED on/off
 */
void str8100_led_toggle(unsigned int led_index)
{     
    volatile unsigned int    data_out_state;


    /*
     * 1. read GPIO Data Out State
     * 2. if GPIO High, turn LED on, otherwise, turn LED off
     */
    data_out_state = GPIO_DATA_OUTPUT_REG;
    
    if (data_out_state & led_index& LED_MASK)
    {
        // GPIO High, i.e., LED is off. Now, turn it on
        str8100_led_on(led_index & LED_MASK);
    }
    else
    {
        // GPIO Low, i.e., LED is on. Now turn it off
        str8100_led_off(led_index & LED_MASK);
    }
}



/*
 * Initilaize LED settings
 */
void str8100_led_init(void)
{


    /*
     * Configure all GPIO pins as follows:
     * 1. output pins
     * 2. turn all leds off
     */
    printk("%s: \n",__FUNCTION__);

    GPIO_DIRECTION_REG |= LED_MASK;
    
    str8100_led_all_off();

}
/*
 * ===============================================================================
 *  DRIVER FUNCTIONS:
 *  Copyright (c) 2008 Cavium Networks 
 * ===============================================================================
 */
/*
 * This function is called when the module is loaded
 */
static int __init ledmod_start(void)
{
	//mode_t mode;
	//struct device *err_dev;
	str8100_led_init(); // Turn off all led on startup.

        Major = register_chrdev(0, DEVICE_NAME, &fops); // The major number of the driver.

        if (Major < 0) {
          printk(KERN_ALERT "Registering NAS 652 led device driver failed with %d\n", Major);
          return Major;
        }

 
        /* create /dev/pi
         * we use udev to make the file
         */
        //driverled_class = class_create(THIS_MODULE,DEVICE_NAME);

	//device_create(lp_class, port->dev, MKDEV(LP_MAJOR, nr), "lp%d", nr);
        //err_dev = device_create(driverled_class, NULL, MKDEV(Major,0),DEVICE_NAME,Major);
	//device_create(driverled_class, NULL, MKDEV(Major,0),"Driverled%d",Major);
	//if (mknod("/dev/driverled", mode |= S_IFCHR, MKDEV(Major,0))){
	//    printk(KERN_ALERT "Failed to create device file");
	//}

        printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
        printk(KERN_INFO "the driver, create a dev file with\n");
        printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
        printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
        printk(KERN_INFO "the device file.\n");
        printk(KERN_INFO "Remove the device file and module when done.\n");

        return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
static void __exit ledmod_end(void)
{
        /* 
         * Unregister the device 
         */
	str8100_led_all_off(); // Turn off all led on cleanup.

	printk(KERN_INFO "Unregistering NAS 652 led device driver.\n");
	//device_destroy(driverled_class,MKDEV(Major,0));
	//class_unregister(driverled_class);
	//class_destroy(driverled_class);

	unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
        static int counter = 0;

        if (Device_Open)
                return -EBUSY;

        Device_Open++;
        sprintf(msg, "I already told you %d times Hello world!\n", counter++);
        msg_Ptr = msg;

	//str8100_led_init();

        try_module_get(THIS_MODULE);

        return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
        Device_Open--;          /* We're now ready for our next caller */

        /* 
         * Decrement the usage count, or else once you opened the file, you'll
         * never get get rid of the module. 
         */
        module_put(THIS_MODULE);

        return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,   /* see include/linux/fs.h   */
                           char *buffer,        /* buffer to fill with data */
                           size_t length,       /* length of the buffer     */
                           loff_t * offset)
{
        /*
         * Number of bytes actually written to the buffer 
         */
        int bytes_read = 0;

        /*
         * If we're at the end of the message, 
         * return 0 signifying end of file 
         */
        if (*msg_Ptr == 0)
                return 0;

        /* 
         * Actually put the data into the buffer 
         */
        while (length && *msg_Ptr) {

                /* 
                 * The buffer is in the user data segment, not the kernel 
                 * segment so "*" assignment won't work.  We have to use 
                 * put_user which copies data from the kernel data segment to
                 * the user data segment. 
                 */
                put_user(*(msg_Ptr++), buffer++);

                length--;
                bytes_read++;
        }

        /* 
         * Most read functions return the number of bytes put into the buffer
         */
        return bytes_read;
}

/*  
 * Called when a process writes to dev file
 * Writes 
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{

/*
  Action: 4 most significant bits:
  0010 = (2) togle
  0011 = (3) turn all leds off
  0100 = (4) turn all leds on
  0101 = (5) turn a led on
  0110 = (6) turn a led off 

  Led number: 4 less significant bits:
  0001 = led 1
  0010 = led 2
  0011 = led 3
*/
	unsigned int ledlist[]={1,16,17};
	unsigned char tmp;
	unsigned char action;
	unsigned char leds;
	int ret;
	unsigned int led;
	
	ret = get_user(tmp, buff);
//	copy_from_user(&tmp,buff,len);

//	while (len){ 
	printk(KERN_INFO "Message writed: %c\n",tmp);
	printk(KERN_INFO "Lenght: %d\n",len);

	action = tmp & 0xF0;
	leds = tmp & 0x0F;

	led = 1<<ledlist[leds]&LED_MASK;
	led = 1 << ledlist[leds];


	switch (action){
	    case 0x50: str8100_led_on(led); break;
	    case 0x60: str8100_led_off(led); break;
	    case 0x20: str8100_led_toggle(led); break;
	    case 0x30: str8100_led_all_off(); break;
	    case 0x40: str8100_led_all_on(); break;
	    default: str8100_led_all_off();
	}

       
        return -EINVAL;
}

module_init(ledmod_start);
module_exit(ledmod_end);
