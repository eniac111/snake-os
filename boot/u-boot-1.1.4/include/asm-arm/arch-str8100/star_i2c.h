/*======================================================================

   Copyright (C) 2006 STAR Semiconductor Limited

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

======================================================================*/

#ifndef _STAR_I2C_H_
#define _STAR_I2C_H_

#include <asm/types.h>

#include <asm/arch/star_sys_memory_map.h>

//#define I2C_MEM_MAP_VALUE(reg_offset)         (*((unsigned int volatile *)I2C_MEM_MAP_ADDR(reg_offset)))
//#define I2C_MEM_MAP_ADDR(reg_offset)          (SYSVA_I2C_BASE_ADDR + reg_offset) 

#if defined(__UBOOT__)
#define I2C_MEM_MAP_VALUE(reg_offset)          (*((u32 volatile *)(SYSPA_I2C_BASE_ADDR                     + reg_offset)))
#elif defined(__LINUX__)
#define I2C_MEM_MAP_VALUE(reg_offset)          (*((u32 volatile *)(SYSVA_I2C_BASE_ADDR    + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif






#define I2C_CONTROLLER_REG_ADDR               I2C_MEM_MAP_ADDR(0x20)
#define I2C_TIME_OUT_REG_ADDR                 I2C_MEM_MAP_ADDR(0x24)
#define I2C_SLAVE_ADDRESS_REG_ADDR            I2C_MEM_MAP_ADDR(0x28)
#define I2C_WRITE_DATA_REG_ADDR               I2C_MEM_MAP_ADDR(0x2C)
#define I2C_READ_DATA_REG_ADDR                I2C_MEM_MAP_ADDR(0x30)
#define I2C_INTERRUPT_STATUS_REG_ADDR         I2C_MEM_MAP_ADDR(0x34)
#define I2C_INTERRUPT_ENABLE_REG_ADDR         I2C_MEM_MAP_ADDR(0x38)

#define I2C_CONTROLLER_REG                    I2C_MEM_MAP_VALUE(0x20)
#define I2C_TIME_OUT_REG                      I2C_MEM_MAP_VALUE(0x24)
#define I2C_SLAVE_ADDRESS_REG                 I2C_MEM_MAP_VALUE(0x28)
#define I2C_WRITE_DATA_REG                    I2C_MEM_MAP_VALUE(0x2C)
#define I2C_READ_DATA_REG                     I2C_MEM_MAP_VALUE(0x30)
#define I2C_INTERRUPT_STATUS_REG              I2C_MEM_MAP_VALUE(0x34)
#define I2C_INTERRUPT_ENABLE_REG              I2C_MEM_MAP_VALUE(0x38)

#define I2C_READ_ONLY_CMD      (0)
#define I2C_WRITE_ONLY_CMD     (1)
#define I2C_WRITE_READ_CMD     (2)
#define I2C_READ_WRITE_CMD     (3)

#define I2C_DATA_LEN_1_BYTE    (0)
#define I2C_DATA_LEN_2_BYTE    (1)
#define I2C_DATA_LEN_3_BYTE    (2)
#define I2C_DATA_LEN_4_BYTE    (3)

#define I2C_BUS_ERROR_FLAG     (0x1)
#define I2C_ACTION_DONE_FLAG   (0x2)

#define HAL_I2C_ENABLE_I2C()          (I2C_CONTROLLER_REG) |= ((unsigned int)0x1 << 31); 
#define HAL_I2C_DISABLE_I2C()         (I2C_CONTROLLER_REG) &= ~((unsigned int)0x1 << 31);
#define HAL_I2C_ENABLE_DATA_SWAP()    (I2C_CONTROLLER_REG) |= (0x1 << 24); 
#define HAL_I2C_DISABLE_DATA_SWAP()   (I2C_CONTROLLER_REG) &= ~(0x1 << 24); 
#define HAL_I2C_START_TRANSFER()      (I2C_CONTROLLER_REG) |= (0x1 << 6); 
#define HAL_I2C_STOP_TRANSFER()       (I2C_CONTROLLER_REG) &= ~(0x1 << 6); 

typedef struct _i2c_transfer_s_
{
    u32    transfer_cmd;
    
    u32    write_data_len;
    
    u32    read_data_len;
    
    u32    write_data;
    
    u32    read_data;
    
    u32    slave_addr;
    
    u32    action_done;
    
    u32    error_status;
}i2c_transfer_t;


typedef struct _I2C_OBJECT_S_
{
    u32          control_reg;
    
    u32          time_out_reg;
    
    u32          interrupt_config;
    
    /* 
 *      * For interrupt setting
 *           */
    //INTC_OBJECT_T    intc_obj;
}I2C_OBJECT_T;


#endif

