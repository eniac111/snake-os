/*
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/arch/star_uart.h>
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_misc.h>

void star_serial_config_baud_rate(u32 uart_port_no, u32 baud_rate, u32 input_clock)
{
	u32 baud_rate_divisor;
	u32 updated_input_clock;
	u32 DLM, DLL;

#if 0 // on ASIC
	HAL_UART_WRITE_PSR(uart_port_no, 1);
	updated_input_clock = UART_INPUT_CLOCK_24M;
#else // on FPGA
	if (baud_rate == 1500000) {
		HAL_UART_WRITE_PSR(uart_port_no, 1);
		updated_input_clock = UART_INPUT_CLOCK_24M;
	} else {
		HAL_UART_WRITE_PSR(uart_port_no, 2);
		updated_input_clock = UART_INPUT_CLOCK_13M;
	}
#endif

	baud_rate_divisor = updated_input_clock / (16 * baud_rate);

	DLM = (baud_rate_divisor & 0x0000FF00) >> 8;
	DLL = (baud_rate_divisor & 0x000000FF);
    
	if (baud_rate_divisor) {    
		HAL_UART_WRITE_DLM(uart_port_no, DLM);
		HAL_UART_WRITE_DLL(uart_port_no, DLL);
	}
}

void star_serial_config_line_control(u32 uart_port_no, u32 word_len, u32 parity, u32 stop_bit)
{
	u32 uart_LCR = 0;

	// configure word length setting
	switch (word_len) {
	case WORD_FIVE_BITS:
		uart_LCR |= WORD_LENGTH_5;
		break;
		
	case WORD_SIX_BITS:
		uart_LCR |= WORD_LENGTH_6;
		break;
		
	case WORD_SEVEN_BITS:
		uart_LCR |= WORD_LENGTH_7;
		break;

	case WORD_EIGHT_BITS:
		uart_LCR |= WORD_LENGTH_8;
		break;

	default:
		break;
	}
    
	// configure parity setting
	switch (parity) {
	case NONE_PARITY:
		uart_LCR |= PARITY_CHECK_NONE;
		break;
	
	case EVEN_PARITY:
		uart_LCR |= PARITY_CHECK_EVEN;
		break;
		
	case ODD_PARITY:
		uart_LCR |= PARITY_CHECK_ODD;
		break;

	case ONE_PARITY:
		uart_LCR |= PARITY_CHECK_STICK_ONE;
		break;

	case ZERO_PARITY:
		uart_LCR |= PARITY_CHECK_STICK_ZERO;
		break;

	default:
		break;
	}

	// configure stop bit setting
	if (stop_bit == ONE_STOP_BIT) {
		uart_LCR |= STOP_BIT_1;
	} else if ((stop_bit == ONE_HALF_STOP_BIT) || (stop_bit == TWO_STOP_BIT)) {
		uart_LCR |= STOP_BIT_2;
	}

	_UART_LCR(uart_port_no) = uart_LCR;
}

void star_serial_config_fifo(u32 uart_port_no, u32 fifo_flag, u32 rx_fifo_trigger_level)
{
	u32 uart_FCR = 0;

	if (fifo_flag == TX_RX_FIFO_DISABLE) {
		uart_FCR = 0;
	} else {
		uart_FCR |= FIFO_ENABLE;

		if (rx_fifo_trigger_level == 1) {
			uart_FCR |= RX_FIFO_TRIGGER_LEVEL_1;
		} else if (rx_fifo_trigger_level == 4) {
			uart_FCR |= RX_FIFO_TRIGGER_LEVEL_4;
		} else if (rx_fifo_trigger_level == 8) {
			uart_FCR |= RX_FIFO_TRIGGER_LEVEL_8;
		} else if (rx_fifo_trigger_level == 14) {
			uart_FCR |= RX_FIFO_TRIGGER_LEVEL_14;
		}
		// reset TX/RX FIFO
		uart_FCR |= (RX_FIFO_RESET | TX_FIFO_RESET);

		// TX trigger level
		//uart_FCR |= TX_FIFO_TRIGGER_LEVEL_9;
	}

	_UART_FCR(uart_port_no) = uart_FCR;
}

void star_serial_hw_init(void)
{
#if 1
	HAL_MISC_ENABLE_UART1_TXD_PIN();
	HAL_MISC_ENABLE_UART1_RXD_PIN();
	HAL_MISC_ENABLE_UART1_RTS_PIN();
	HAL_MISC_ENABLE_UART1_CTS_PIN();

	HAL_PWRMGT_ENABLE_UART0_CLOCK();
	HAL_PWRMGT_ENABLE_UART1_CLOCK();
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (1 << PWRMGT_UART0_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(1 << PWRMGT_UART0_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (1 << PWRMGT_UART0_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (1 << PWRMGT_UART1_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(1 << PWRMGT_UART1_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (1 << PWRMGT_UART1_SOFTWARE_RESET_BIT_INDEX);
#endif
}

void serial_setbrg(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	star_serial_hw_init();
	star_serial_config_baud_rate(0, gd->baudrate, UART_INPUT_CLOCK);
	star_serial_config_line_control(0, WORD_EIGHT_BITS, NONE_PARITY, ONE_STOP_BIT);
	star_serial_config_fifo(0, TX_RX_FIFO_ENABLE, 8);

	star_serial_config_baud_rate(1, gd->baudrate, UART_INPUT_CLOCK);
	star_serial_config_line_control(1, WORD_EIGHT_BITS, NONE_PARITY, ONE_STOP_BIT);
	star_serial_config_fifo(1, TX_RX_FIFO_ENABLE, 8);
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
int serial_init(void)
{
	serial_setbrg();

	return 0;
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc(void)
{
	char rx_data;

	while (!HAL_UART_CHECK_RX_DATA_READY(CONFIG_CONSOLE_UART_PORT))
		; /* wait */

	HAL_UART_READ_DATA(CONFIG_CONSOLE_UART_PORT, rx_data);

	return rx_data;
}

/*
 * Output a single byte to the serial port.
 */
void serial_putc(const char c)
{
	while (!HAL_UART_CHECK_TX_FIFO_EMPTY(CONFIG_CONSOLE_UART_PORT))
		; /* wait */

	HAL_UART_WRITE_DATA(CONFIG_CONSOLE_UART_PORT, c);
	if (c == '\n') {
		serial_putc('\r');
	}
}

/*
 * Test whether a character is in the RX buffer
 */
int serial_tstc(void)
{
	return (HAL_UART_CHECK_RX_DATA_READY(CONFIG_CONSOLE_UART_PORT));
}

void
serial_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

