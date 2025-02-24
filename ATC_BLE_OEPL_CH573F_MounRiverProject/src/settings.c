#include "tl_common.h"

#define EEPROM_ADDRESS_SETTINGS 0x79000
#define MAGIC_WORD_DEFAULT 0xABDC1234
#define MAGIC_VERSION_DEFAULT 1
#define MAGIC_WORD 0xABDC0E97
#define MAGIC_VERSION 17

#define EPD_CTRL_NONE 0
#define EPD_CTRL_UC 1
#define EPD_CTRL_SSD 2
#define EPD_CTRL_ST 3
#define EPD_CTRL_TI 4
#define EPD_CTRL_UC_PRO 5

settings_struct settings = {};
settings_default_type_struct settings_default;


typedef struct Default_settings_struct
{
	uint16_t hw_type;
	uint16_t screen_functions;
	uint8_t screen_w_h_inversed_ble;
	uint16_t screen_w_h_inversed;
	uint16_t screen_h;
	uint16_t screen_w;
	uint16_t screen_h_offset;
	uint16_t screen_w_offset;
	uint16_t screen_colors;
	uint16_t screen_black_invert;
	uint16_t screen_second_color_invert;
} default_settings_struct;

const default_settings_struct default_setting[MAX_LEN_TYPE] = {
	{0xFA, EPD_CTRL_NONE, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
	{0x57, EPD_CTRL_SSD, 1, 0, 200, 200, 0, 0, 2, 1, 0},
	/*{0x60, EPD_CTRL_UC, 0, 0, 384, 184, 0, 0, 2, 0, 0, },
	{0x60, EPD_CTRL_UC, 0, 0, 384, 184, 0, 0, 2, 1, 0, },
	{0x60, EPD_CTRL_SSD, 0, 1, 384, 184, 0, 0, 2, 1, 0, },
	{0x62, EPD_CTRL_UC, 0, 0, 384, 184, 0, 0, 1, 1, 0, },*/
};


void init_settings(void)
{
}

u8 default_ble_name[] = {'A', 'T', 'C', '_', '0', '0', '0', '0', '0', '0'};

void settings_set_screen_data(uint16_t screen_type_in)
{
	printf("settings_set_screen_data %u\r\n", screen_type_in);
	if (screen_type_in < MAX_LEN_TYPE)
	{
		{// Default Display type in a special setting
			settings_default.magic = MAGIC_WORD_DEFAULT;
			settings_default.version = MAGIC_VERSION_DEFAULT;
			settings_default.len = sizeof(settings_default_type_struct);
			settings_default.oepl_hw_type = screen_type_in;
			settings_default.crc = get_crc_default();
		}
		settings.oepl_hw_type = default_setting[screen_type_in].hw_type;
		settings.screen_available = screen_type_in ? 1 : 0;
		settings.screen_type = screen_type_in;
		settings.screen_functions = default_setting[screen_type_in].screen_functions;
		settings.screen_w_h_inversed_ble = default_setting[screen_type_in].screen_w_h_inversed_ble;
		settings.screen_w_h_inversed = default_setting[screen_type_in].screen_w_h_inversed;
		settings.screen_h = default_setting[screen_type_in].screen_h;
		settings.screen_w = default_setting[screen_type_in].screen_w;
		settings.screen_h_offset = default_setting[screen_type_in].screen_h_offset;
		settings.screen_w_offset = default_setting[screen_type_in].screen_w_offset;
		settings.screen_colors = default_setting[screen_type_in].screen_colors;
		settings.screen_color_black_invert = default_setting[screen_type_in].screen_black_invert;
		settings.screen_color_second_invert = default_setting[screen_type_in].screen_second_color_invert;
		uint32_t calc_img_each = 0;
		uint8_t color_count = 2;
		if(settings.screen_colors>2)
			color_count = settings.screen_colors;
		calc_img_each = ((settings.screen_h * settings.screen_w) / 8) * color_count/*WE USE minimum 3 Colors all the time as an upload can still be 3 colors*/ + sizeof(struct EepromImageHeader) + 0x100;
		calc_img_each = (calc_img_each + EEPROM_PAGE_SIZE - 1) / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE;
		if (calc_img_each > EEPROM_IMG_LEN || (settings.screen_h == 0 && settings.screen_w == 0))
		{ // In case of error lets just reset to one slot
			calc_img_each = EEPROM_IMG_LEN;
		}
		settings.saved_EEPROM_IMG_EACH = calc_img_each;
	}
}

void reset_settings_to_default(uint16_t screen_type_in)
{
	settings.magic = MAGIC_WORD;
	settings.version = MAGIC_VERSION;
	settings.len = sizeof(settings_struct);

	settings_set_screen_data(screen_type_in);

	settings.oepl_enabled = 1;
	settings.oepl_wakeup_time = 40;
	settings.ble_enabled = 1;
	settings.ble_wakeup_time = 3200;
	settings.nfc_available = 0;
	settings.button_available = 0;
	settings.led_available = 3;
}

uint8_t get_crc_setting(void)
{
	uint8_t temp_crc = 0x00;

	for (int i = 0; i < sizeof(settings_struct) - 1; i++) // Iterate over everything expect the last value as it is CRC itself
	{
		temp_crc = temp_crc ^ ((uint8_t *)&settings)[i];
	}
	return temp_crc;
}

uint8_t get_crc_default(void)
{
	uint8_t temp_crc = 0x00;

	for (int i = 0; i < sizeof(settings_default_type_struct) - 1; i++) // Iterate over everything expect the last value as it is CRC itself
	{
		temp_crc = temp_crc ^ ((uint8_t *)&settings_default)[i];
	}
	return temp_crc;
}
