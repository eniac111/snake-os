
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_misc.h>
#include <asm/arch-str8100/star_i2c.h>

#define LCM_SD1602X_SLAVE_ADDR_ID    (0x3A)

#define LCM_SD1602X_INSTRUCTION_REG  (0)

#define LCM_SD1602X_DATA_REG         (1)


static i2c_transfer_t i2c_cmd_transfer;

u32 Hal_I2c_Is_Action_Done(void)
{
	/*
	 * Return value :
	 *    1 : Action Done
	 *    0 : Action is NOT Done
	 */
	return ((I2C_INTERRUPT_STATUS_REG & I2C_ACTION_DONE_FLAG) ? 1 : 0);
}

u32 Hal_I2c_Is_Bus_Idle(void)
{
	/*
	 * Return value :
	 *    1 : Bus Idle
	 *    0 : Bus Busy
	 */
	return ((I2C_CONTROLLER_REG & (0x1 << 6)) ? 0 : 1);
}


//#define SHOW_I2C_CMD
void Hal_I2c_Dispatch_Transfer(i2c_transfer_t * i2c_transfer)
{
	u32 volatile i2c_control;
	u32 volatile i2c_control_reg;


	/*
	 * Wait unti I2C Bus is idle and the previous action is done
	 */
	while (!Hal_I2c_Is_Bus_Idle() && !Hal_I2c_Is_Action_Done());


	// Configure transfer command, write data length, and read data length
	i2c_control = ((i2c_transfer->transfer_cmd & 0x3) << 4) |
	    ((i2c_transfer->write_data_len & 0x3) << 2) |
	    ((i2c_transfer->read_data_len & 0x3) << 0);

	// Note we enable I2C again!!
	i2c_control_reg = I2C_CONTROLLER_REG;

	i2c_control_reg &= ~(0x3F);
	i2c_control_reg |= (i2c_control & 0x3F) | ((u32) 0x1 << 31);

	I2C_CONTROLLER_REG = i2c_control_reg;


	// Write output data
	I2C_WRITE_DATA_REG = i2c_transfer->write_data;

	// Configure slave address
	I2C_SLAVE_ADDRESS_REG = i2c_transfer->slave_addr & 0xFE;

#ifdef SHOW_I2C_CMD
	printf("in Hal_I2c_Dispatch_Transfer\n");
	printf("I2C_CONTROLLER_REG : %x\n", I2C_CONTROLLER_REG);
	printf("I2C_WRITE_DATA_REG : %x\n", I2C_WRITE_DATA_REG);
	printf("I2C_SLAVE_ADDRESS_REG : %x\n", I2C_SLAVE_ADDRESS_REG);
#endif

	// Start IC transfer
	HAL_I2C_START_TRANSFER();
}



u32
I2c_Write_Only_Command(u32 slave_addr, u32 write_data_len, u32 write_data)
{
	u32 cpsr_flags;

#if 0
	Sys_Interrupt_Disable_Save_Flags(&cpsr_flags);

	// Clear previous I2C interrupt status
	IO_OUT_WORD(I2C_INTERRUPT_STATUS_REG_ADDR,
		    I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);

	// Enable I2C interrupt sources
	IO_OUT_WORD(I2C_INTERRUPT_ENABLE_REG_ADDR,
		    I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);
#endif
	I2C_INTERRUPT_STATUS_REG =
	    (I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);

	/*
	 * Configure this I2C command tranfer settings
	 */
	i2c_cmd_transfer.transfer_cmd = I2C_WRITE_ONLY_CMD;

	i2c_cmd_transfer.write_data_len = write_data_len & 0x3;

	i2c_cmd_transfer.read_data_len = 0;

	i2c_cmd_transfer.slave_addr = slave_addr & 0xFF;

	switch (write_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:

		i2c_cmd_transfer.write_data = write_data & 0xFF;

		break;

	case I2C_DATA_LEN_2_BYTE:

		i2c_cmd_transfer.write_data = write_data & 0xFFFF;

		break;

	case I2C_DATA_LEN_3_BYTE:

		i2c_cmd_transfer.write_data = write_data & 0xFFFFFF;

		break;

	case I2C_DATA_LEN_4_BYTE:

		i2c_cmd_transfer.write_data = write_data;

	default:

		i2c_cmd_transfer.write_data = write_data;

		break;
	}

	i2c_cmd_transfer.error_status = 0;

	i2c_cmd_transfer.action_done = 0;

	/*
	 * Issue this command
	 */
	Hal_I2c_Dispatch_Transfer(&i2c_cmd_transfer);

	//Sys_Interrupt_Restore_Flags(cpsr_flags);

	return 0;
}



u32 I2c_Lcm_Sd1602x_Function_Set_Command(u32 function_set_command_byte)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_INSTRUCTION_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 */
	i2c_write_data =
	    (control_byte & 0xFF) | ((function_set_command_byte & 0xFF) <<
				     8);
	//printf("control_byte: %x\n", control_byte);
	//printf("function_set_command_byte: %x\n", function_set_command_byte);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);


	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}










u32 I2c_Lcm_Sd1602x_Write_Instruction_Command(u32 command_byte)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_INSTRUCTION_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 */
	i2c_write_data =
	    (control_byte & 0xFF) | ((command_byte & 0xFF) << 8);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}

u32 I2c_Lcm_Sd1602x_Set_Entry_Mode_Command(u32 entry_mode_command_byte)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_INSTRUCTION_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 */
	i2c_write_data =
	    (control_byte & 0xFF) | ((entry_mode_command_byte & 0xFF) <<
				     8);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}


u32 I2c_Lcm_Sd1602x_Clear_Display_Command(void)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_INSTRUCTION_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 command_byte = 0x1;
	u32 slave_addr;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 */
	i2c_write_data =
	    (control_byte & 0xFF) | ((command_byte & 0xFF) << 8);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register (offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}

u32
I2c_Lcm_Sd1602x_Display_Control_Command(u32 display_control_command_byte)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_INSTRUCTION_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 */
	i2c_write_data =
	    (control_byte & 0xFF) | ((display_control_command_byte & 0xFF)
				     << 8);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}


u32 I2c_Lcm_Sd1602x_Init(void)
{
	u32 status = 0;
	u32 cmd_data;



	/*
	 * Wait at least 2ms after external reset has been applied.
	 */
	//Hal_Timer_Timer3_Delay(5 * 1000);
	udelay(5 * 1000);


	/*
	 * Function set command : 8-bit, H = 0
	 */
	cmd_data = (0x1 << 5) | (0x1 << 4) | (0x0 << 0);

	if ((status = I2c_Lcm_Sd1602x_Function_Set_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: function set command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}

	/*
	 * Wait at least 2ms
	 */
	//Hal_Timer_Timer3_Delay(5 * 1000);
	udelay(5 * 1000);

	cmd_data = (0x1 << 5) | (0x1 << 4);

	if ((status = I2c_Lcm_Sd1602x_Function_Set_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: function set command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}

	/*
	 * Wait at least 40us
	 */
	//Hal_Timer_Timer3_Delay(100);
	udelay(100);

	/*
	 * Function set command : 8-bit, H = 0
	 */
	cmd_data = (0x1 << 5) | (0x1 << 4) | (0x0 << 0);

	if ((status = I2c_Lcm_Sd1602x_Function_Set_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: function set command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	cmd_data = (0x1 << 5) | (0x1 << 4);

	if ((status = I2c_Lcm_Sd1602x_Function_Set_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: function set command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Display control command : off
	 */
	cmd_data = (0x1 << 3);

	if ((status =
	     I2c_Lcm_Sd1602x_Display_Control_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: display control command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Clear display command
	 */
	if ((status = I2c_Lcm_Sd1602x_Clear_Display_Command()) != 0) {
		printf
		    ("\nI2C: clear display command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Set entry mode command : incremental
	 */
	cmd_data = (0x1 << 2) | (0x1 << 1);

	if ((status =
	     I2c_Lcm_Sd1602x_Set_Entry_Mode_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: set entry mode command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);


	/*
	 * Display control command : display on (bit2), cursor on (bit1)
	 */
	cmd_data = (0x1 << 3) | (0x1 << 2) | (0x1 << 1);

	if ((status =
	     I2c_Lcm_Sd1602x_Display_Control_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: display control command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Function set command : 8-bit, H = 1
	 */
	cmd_data = (0x1 << 5) | (0x1 << 4) | (0x1 << 0);

	if ((status = I2c_Lcm_Sd1602x_Function_Set_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: function set command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Display configuration command : P = 0, Q = 0
	 */
	cmd_data = (0x1 << 2);

	if ((status =
	     I2c_Lcm_Sd1602x_Write_Instruction_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: display configuration command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Temperature Control command : TC1 = 1, TC2 = 1
	 */
	cmd_data = (0x1 << 4) | (0x1 << 1) | (0x1 << 0);

	if ((status =
	     I2c_Lcm_Sd1602x_Write_Instruction_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: temperature control command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Set VLCD command : VA = 0x27 ?????, and VLCD = 5.0V
	 */
	cmd_data = 0x9B;

	if ((status =
	     I2c_Lcm_Sd1602x_Write_Instruction_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: set VLCD command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);

	/*
	 * Function set command : 8-bit, H = 0
	 */
	cmd_data = (0x1 << 5) | (0x1 << 4) | (0x0 << 0);

	if ((status = I2c_Lcm_Sd1602x_Function_Set_Command(cmd_data)) != 0) {
		printf
		    ("\nI2C: function set command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}
	//Hal_Timer_Timer3_Delay(1 * 1000);
	udelay(1000);
	status = 1;

	return status;
}

u32 I2c_Lcm_Sd1602x_Set_DDRAM_Address_Command(u32 ddram_adress);

int i2c_lcm_sd1602x_move_x(int x)
{
	return I2c_Lcm_Sd1602x_Set_DDRAM_Address_Command(x);
}

u32 I2c_Lcm_Sd1602x_Set_DDRAM_Address_Command(u32 ddram_adress)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_INSTRUCTION_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 command_byte;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 */
	command_byte = (0x1 << 7) | (ddram_adress & 0x7F);

	i2c_write_data =
	    (control_byte & 0xFF) | ((command_byte & 0xFF) << 8);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}


u32 I2c_Lcm_Sd1602x_Write_Data_Command(u32 write_data)
{
	u32 i2c_write_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_DATA_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 command_byte;
	u32 cmd_status;

	/*
	 * The i2c_write_data is composed of "Control Byte" and "Command Byte".
	 * Note whether the CGRAM or DDRAM is to be written into is determined by 
	 * the previous Set CGRAM address or Set DDRAM address command.
	 * For CGRAM data, only bits D4 to D0 of CGRAM data are valid. Bit D7 to D5
	 * are don't care.
	 */
	command_byte = (write_data & 0xFF);

	i2c_write_data =
	    (control_byte & 0xFF) | ((command_byte & 0xFF) << 8);

	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Write_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				   i2c_write_data);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}




u32
I2c_Read_Only_Command(u32 slave_addr, u32 read_data_len, u32 * read_data)
{
	u32 cpsr_flags;

#if 0
	// Clear previous I2C interrupt status
	IO_OUT_WORD(I2C_INTERRUPT_STATUS_REG_ADDR,
		    I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);

	// Enable I2C interrupt sources
	IO_OUT_WORD(I2C_INTERRUPT_ENABLE_REG_ADDR,
		    I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);
#endif
	I2C_INTERRUPT_STATUS_REG =
	    (I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);
	I2C_INTERRUPT_ENABLE_REG =
	    (I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG);

	/*
	 * Configure this I2C command tranfer settings
	 */
	i2c_cmd_transfer.transfer_cmd = I2C_READ_ONLY_CMD;

	i2c_cmd_transfer.write_data_len = 0;

	i2c_cmd_transfer.read_data_len = read_data_len & 0x3;

	i2c_cmd_transfer.slave_addr = slave_addr & 0xFF;

	i2c_cmd_transfer.write_data = 0;

	i2c_cmd_transfer.error_status = 0;

	i2c_cmd_transfer.action_done = 0;


	/*
	 * Issue this command
	 */
	Hal_I2c_Dispatch_Transfer(&i2c_cmd_transfer);


	// Check if this I2C bus action is done or not
	while (1) {
		u32 i2c_interrupt_status_reg = I2C_INTERRUPT_STATUS_REG;
		printf("Orz\n");
		//Sys_Interrupt_Disable_Save_Flags(&cpsr_flags);
		if (((i2c_interrupt_status_reg >> 1) & 0x1) == 1) {
			break;
		} else {
			printf("I2C_INTERRUPT_STATUS_REG : %x\n",
			       I2C_INTERRUPT_STATUS_REG);
		}
#if 0
		if ((i2c_cmd_transfer.action_done)
		    || (i2c_cmd_transfer.error_status)) {
			break;
		}
#endif
		//Sys_Interrupt_Restore_Flags(cpsr_flags);
	}

	//Sys_Interrupt_Restore_Flags(cpsr_flags);


	// I2C Bus error!!
	if (i2c_cmd_transfer.error_status
	    && (i2c_cmd_transfer.error_status != 0xFF)) {
		return (i2c_cmd_transfer.error_status);
	}

	// Get the read data byte
	//i2c_cmd_transfer.read_data = IO_IN_WORD(I2C_READ_DATA_REG_ADDR);
	i2c_cmd_transfer.read_data = I2C_READ_DATA_REG;

	switch (read_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:

		i2c_cmd_transfer.read_data &= 0xFF;	//8

		break;

	case I2C_DATA_LEN_2_BYTE:

		i2c_cmd_transfer.read_data &= 0xFFFF;	//16

		break;

	case I2C_DATA_LEN_3_BYTE:

		i2c_cmd_transfer.read_data &= 0xFFFFFF;	//24

		break;

	case I2C_DATA_LEN_4_BYTE:

	default:

		break;
	}


	// Set the data for return
	*read_data = i2c_cmd_transfer.read_data;

	return (0);
}


u32 I2c_Lcm_Sd1602x_Read_Data_Command(u8 * read_data)
{
	u32 i2c_read_data = 0;
	u32 co_bit = 0;
	u32 rs_bit = LCM_SD1602X_DATA_REG;
	u32 control_byte = (co_bit << 7) | (rs_bit << 6);
	u32 slave_addr;
	u32 cmd_status;


	// Note the slave_addr is mapped to hardware I2C Slave_Address_Register(offset 0x28)
	slave_addr = (LCM_SD1602X_SLAVE_ADDR_ID << 1);

	cmd_status =
	    I2c_Read_Only_Command(slave_addr, I2C_DATA_LEN_2_BYTE,
				  &i2c_read_data);

	// for testing only
	printf("\nI2C: i2c_read_data = 0x%08x\n", i2c_read_data);

	/*
	 * The i2c_read_data is composed of "Control Byte" and "Command Byte".
	 */
	*read_data = (u8) ((i2c_read_data >> 8) & 0xFF);

	/*
	 * 0 : command OK; otherwise, command failed
	 */
	return cmd_status;
}



int i2c_lcm_sd1602x_display_put_char(u8 achar)
{
	const u8 char_offset='1'- 0xb1;

	return I2c_Lcm_Sd1602x_Write_Data_Command(achar+char_offset);
}

int i2c_lcm_sd1602x_print(u8 x, const char* str)
{
	int i=0;

	i2c_lcm_sd1602x_move_x(x);

	for (i=0; str[i] != 0 ; ++i){
		i2c_lcm_sd1602x_display_put_char(str[i]);
	}

	return 0;
}

u32 I2c_Lcm_Sd1602x_Display_Test(u8 x, u8 ddram_data)
{
	u32 status = 0;
	u32 ddram_addr = 0;
	//u32             ddram_data = 0xB0;    // character code
	u32 cgram_addr = 0;
	u32 volatile ii;
	u8 ddram_data8;




	printf("in I2c_Lcm_Sd1602x_Display_Test: %x\n", ddram_data);
	/*
	 * Perform DDRAM test
	 */
	//ddram_data = 0xB1;

	I2c_Lcm_Sd1602x_Set_DDRAM_Address_Command(x);

	for (ii = 0; ii < 1; ii++) {
		if ((status =
		     I2c_Lcm_Sd1602x_Write_Data_Command(ddram_data)) !=
		    0) {
			printf
			    ("\nI2C: write data command failed @ I2C command status = 0x%08x\n",
			     status);

			return status;
		}
		//ddram_data++;

		//Hal_Timer_Timer3_Delay(1 * 1000);
		udelay(1000);
	}

	I2c_Lcm_Sd1602x_Set_DDRAM_Address_Command(x);
	if ((status =
	     I2c_Lcm_Sd1602x_Read_Data_Command(&ddram_data8)) != 0) {
		printf
		    ("\nI2C: read data command failed @ I2C command status = 0x%08x\n",
		     status);

		return status;
	}



	status = 1;

	return status;
}
