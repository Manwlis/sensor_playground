/*
 * LIS3DHTR.c
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "LIS3DHTR.h"
#include "i2c.h"
#include "cmsis_os2.h"

/* Variables ---------------------------------------------------------*/
const LIS3DHTR_reg_t LIS3DHTR_memory_map[LIS3DHTR_NUM_REGS] =
{
#ifdef DEBUG_LIS3DHTR
#define X( name , address , access ) { #name , address , access } ,
#else
#define X( name , address , access ) { address , access } ,
#endif
#include "LIS3DHTR_mmap.def"
#undef X
};

void test_infastructure( const LIS3DHTR_device_t* const device ); // TODO remove
/* Functions ---------------------------------------------------------*/
void LIS3DHTR_print_mmap()
{
	for( int i = 0 ; i < LIS3DHTR_NUM_REGS ; i++ )
	{
#ifdef DEBUG_LIS3DHTR
		printf( "%2d    %20s    %2x    %d\n" , i , LIS3DHTR_memory_map[i].name , LIS3DHTR_memory_map[i].address ,
		        LIS3DHTR_memory_map[i].access );
#else
		printf( "%2d    %2x    %d\n" , i , LIS3DHTR_memory_map[i].address , LIS3DHTR_memory_map[i].access );
#endif
	}
}

void LIS3DHTR_read_all_regs( const LIS3DHTR_device_t* const device )
{
	for( uint8_t i = 0 ; i < LIS3DHTR_NUM_REGS ; i++ )
	{
		uint8_t data = 0xFF;

		LIS3DHTR_read_reg( device , i , &data );

		printf( "%2d    %20s    %x\n" , i , LIS3DHTR_memory_map[i].name , data );
	}
}

void test_infastructure( const LIS3DHTR_device_t* const device )
{
	uint8_t data = 0xFF;

	LIS3DHTR_read_reg( device , LIS3DHTR_FIFO_SRC_REG , &data );

	printf( "FSS %d\n" , reg_get_field( data , FIFO_SRC_REG_FSS_mask , FIFO_SRC_REG_FSS_pos) );
	printf( "EMPTY %d\n" , reg_get_field( data , FIFO_SRC_REG_EMPTY_mask , FIFO_SRC_REG_EMPTY_pos) );
	printf( "OVRN_FIFO %d\n" , reg_get_field( data , FIFO_SRC_REG_OVRN_FIFO_mask , FIFO_SRC_REG_OVRN_FIFO_pos) );
	printf( "WTM %d\n" , reg_get_field( data , FIFO_SRC_REG_WTM_mask , FIFO_SRC_REG_WTM_pos) );

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG1 , &data );

	printf("Value read ------------------\n");
	printf( "LIS3DHTR_CTRL_REG1 %x\n" , data );
	printf("ODR %d\n" , reg_get_field( data , CTRL_REG1_ODR_mask , CTRL_REG1_ODR_pos) );

	reg_set_field( &data , CTRL_REG1_ODR_pos , ODR_100HZ );
	printf("Value to write ------------------\n");
	printf( "LIS3DHTR_CTRL_REG1 %x\n" , data );
	printf("ODR %d\n" , reg_get_field( data , CTRL_REG1_ODR_mask , CTRL_REG1_ODR_pos) );

	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG1 , data );

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG1 , &data );

	printf("Value writen ------------------\n");
	printf( "LIS3DHTR_CTRL_REG1 %x\n" , data );
	printf("ODR %d\n" , reg_get_field( data , CTRL_REG1_ODR_mask , CTRL_REG1_ODR_pos) );
}


void LIS3DHTR_enable_aux_adcs( const LIS3DHTR_device_t* const device )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_TEMP_CFG_REG , &data );
	reg_set_field( &data , TEMP_CFG_ADC_EN_pos , LIS3_ENABLE );
	LIS3DHTR_write_reg( device , LIS3DHTR_TEMP_CFG_REG , data );
}

void LIS3DHTR_disable_aux_adcs( const LIS3DHTR_device_t* const device )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_TEMP_CFG_REG , &data );
	reg_set_field( &data , TEMP_CFG_ADC_EN_pos , LIS3_DISABLE );
	LIS3DHTR_write_reg( device , LIS3DHTR_TEMP_CFG_REG , data );
}

void LIS3DHTR_enable_temp_sensor( const LIS3DHTR_device_t* const device )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_TEMP_CFG_REG , &data );
	reg_set_field( &data , TEMP_CFG_TEMP_EN_pos , LIS3_ENABLE );
	LIS3DHTR_write_reg( device , LIS3DHTR_TEMP_CFG_REG , data );
}

void LIS3DHTR_disable_temp_sensor( const LIS3DHTR_device_t* const device )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_TEMP_CFG_REG , &data );
	reg_set_field( &data , TEMP_CFG_TEMP_EN_pos , LIS3_DISABLE );
	LIS3DHTR_write_reg( device , LIS3DHTR_TEMP_CFG_REG , data );
}

void LIS3DHTR_enable_BDU( const LIS3DHTR_device_t* const device )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG4 , &data );
	reg_set_field( &data , CTRL_REG4_BDU_pos , LIS3_ENABLE );
	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG4 , data );
}

void LIS3DHTR_disable_BDU( const LIS3DHTR_device_t* const device )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG4 , &data );
	reg_set_field( &data , CTRL_REG4_BDU_pos , LIS3_DISABLE );
	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG4 , data );
}

void LIS3DHTR_set_ODR( const LIS3DHTR_device_t* const device , CTRL_REG1_ODR_mode_t ODR_MODE )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG1 , &data );
	reg_set_field( &data , CTRL_REG1_ODR_pos , ODR_MODE );
	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG1 , data );
}

void LIS3DHTR_set_scale( const LIS3DHTR_device_t* const device , CTRL_REG4_FS_setting_t scale )
{
	uint8_t data;

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG4 , &data );
	reg_set_field( &data , CTRL_REG4_FS_pos , scale );
	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG4 , data );
}

void LIS3DHTR_set_resolution( const LIS3DHTR_device_t* const device , LIS3DHTR_output_resolution_t resolution )
{
	uint8_t reg1;
	uint8_t reg4;

	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG1 , &reg1 );
	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG4 , &reg4 );

	switch ( resolution ) {
		case LIS3DHTR_LOW_POWER:
			reg_set_field( &reg1 , CTRL_REG1_LPen_pos , LIS3_ENABLE );
			reg_set_field( &reg4 , CTRL_REG4_HR_pos , LIS3_DISABLE );

			break;
		case LIS3DHTR_NORMAL:
			reg_set_field( &reg1 , CTRL_REG1_LPen_pos , LIS3_DISABLE );
			reg_set_field( &reg4 , CTRL_REG4_HR_pos , LIS3_DISABLE );
			break;
		case LIS3DHTR_HIGH:
			reg_set_field( &reg1 , CTRL_REG1_LPen_pos , LIS3_DISABLE );
			reg_set_field( &reg4 , CTRL_REG4_HR_pos , LIS3_ENABLE );
			break;
		default:
			return; // erroneous setting
	}

	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG1 , reg1 );
	LIS3DHTR_write_reg( device , LIS3DHTR_CTRL_REG4 , reg4 );
}

void LIS3DHTR_get_temp( const LIS3DHTR_device_t* const device )
{
	uint8_t ADC3_H;
	uint8_t ADC3_L;
	int16_t temp_raw;

	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_ADC3_H , &ADC3_H );
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_ADC3_L , &ADC3_L );

	temp_raw = (int16_t)((ADC3_H << 8) | ADC3_L);
	int32_t temp_centi = 2500 + ((int32_t)temp_raw * 100) / 256;


	int32_t temp_int  = temp_centi / 100;
	int32_t temp_frac = temp_centi % 100;

	printf( "temp = %ld.%ld\n" , temp_int , (uint32_t)( ( temp_frac < 0 ) ? -temp_frac : temp_frac ) );
}

void LIS3DHTR_get_acceleration( const LIS3DHTR_device_t* const device , float* x , float* y , float *z )
{
	uint8_t x_l;
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_X_L , &x_l );
	uint8_t x_h;
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_X_H , &x_h );
	int16_t x_raw = (int16_t)( ( x_h << 8 ) | x_l );

	uint8_t y_l;
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_Y_L , &y_l );
	uint8_t y_h;
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_Y_H , &y_h );
	int16_t y_raw = (int16_t)( ( y_h << 8 ) | y_l );

	uint8_t z_l;
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_Z_L , &z_l );
	uint8_t z_h;
	LIS3DHTR_read_reg( device , LIS3DHTR_OUT_Z_H , &z_h );
	int16_t z_raw = (int16_t)( ( z_h << 8 ) | z_l );

	uint8_t data;
	LIS3DHTR_read_reg( device , LIS3DHTR_CTRL_REG4 , &data );
	uint8_t accuracy = reg_get_field( data , CTRL_REG4_FS_mask , CTRL_REG4_FS_pos );

	static const float accuracy_range[FS_NUM_SETTINGS] = { 16000.0f , 7282.0f , 3968.0f , 1280.0f };

	*x = ( (float) x_raw ) / accuracy_range[accuracy];
	*y = ( (float) y_raw ) / accuracy_range[accuracy];
	*z = ( (float) z_raw ) / accuracy_range[accuracy];

	return;
}
