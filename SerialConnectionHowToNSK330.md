This page is about setting up a serial connection to NS-K330.
If you have WLX652 see [this](SerialConnectionHowTo.md) page:

# Hardware setup #
You will need to open your NAS in order to find the serial connection pins.
You will need to connect the pins GND, Tx and Rx to a TTL serial cable.
This means that you MUST NOT connect a STANDARD RS232 serial cable directly!
(The RS232 standard interface uses a voltage range between +5 and +15  volts to represent bit zero and -5 to -15 volts to bit one. This RS232 voltages will burn your NAS.)


**So, you have basically two options:**
  1. Use a serial cable that uses TTL voltage levels;
  1. Build a RS232 to TTL level converter. This is a simple circuit based on the well known and easy to find chip MAX232. (For example: http://pinouts.ru/Converters/rs232-TTL_cable_pinout.shtml )

If you choose the **option 1** -the easiest one- you will find a suitable cable in dealextreme.
(like the one posted by JG: http://www.dealextreme.com/details.dx/sku.13638   (It is about ~5 USD)
This cable is used for Nokia cell phones. It is basically a USB to serial converter without the MAX232 chip to convert the voltage levels to RS232.
In my case my cable has got the following labels CA-50 (CX-U8) cable.

The **picture bellow** shows this cable wired and connected to PCB.

![http://snake-os.googlecode.com/svn/wiki/images/NS_k330_usb_rs232_wireing.jpg](http://snake-os.googlecode.com/svn/wiki/images/NS_k330_usb_rs232_wireing.jpg)

I have cut off the cable end that would connect with the cell phone and changed it by a female pin connector
(like those ones used to connect the front panel LEDs in a PC motherboard).
In the NAS board holes I have soldered the male pins.
The cable colors are -from left to right-

  1. (GND)  Black -  Nearest to CPU black colored on the picture - (CPU is hidden under the coin)
  1. (TX)      Red
  1. (RX)     White from usb cable soldered to yellow wire.
  1. (Vcc)  -Not connected.

You just have to connect the left 3 pins.

_(note by ehabkost, Oct 7th 2010)_ The cable colors from the USB cable may not match the colors on the picture above. For a CA-50 cable bought from Dealextreme on September 2010, the cable colors were: 1. (GND) yellow; 2. (TX) blue; 3. (RX) red.

That is all the hardware you will need to connect your nas on an usb port of your PC.

# Software setup #
I have made this how to using Ubuntu 10.04 - Lucid Lynx.

After plunging the usb-serial cable, your computer will automatically install a new COM port.
Open a terminal, and type "dmesg" command.

```
# dmesg
pl2303 2-2:1.0: pl2303 converter detected
usb 2-2: pl2303 converter now attached to ttyUSB0
```
In my case device was /dev/ttyUSB0

Now you have to set up the connection. In Linux you can use the MINICOM Terminal program. You can install it with
```
# sudo apt-get install minicom
```

Then run minicom with the following parameters:
  * Bits per second: 38400
  * Data bits: 8
  * Parity: None
  * Stop bits: 1

```
# minicom -D /dev/ttyUSB0 -b 38400
```

Your connection is done.

Now you have just to connect the serial cable on NAS and turn it on. You will see on Terminal screen the boot and kernel messages. You can press any key during the uboot start up to have the uboot terminal.

I hope that this how to helps someone.



Regards,<br>
Robert Szabo<br>
Hungary