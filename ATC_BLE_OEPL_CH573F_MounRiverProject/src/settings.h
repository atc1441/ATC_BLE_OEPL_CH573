#pragma once


// MAKE SURE TO INCREASE THE VERSION YOU CHANGE SOMETHING HERE!!! IT COULD END UP BAD OTHERWISE
typedef struct __attribute__ ((packed)) Settings_struct
{
	uint32_t magic;
	uint32_t version;
	uint32_t len;


	uint16_t oepl_hw_type; // We use 16bit here for future use! defaults to 0x00FA to be in a "config" mode

	uint8_t screen_available;
	uint16_t screen_type; // A few different EPD versions are available even for same oepl_hw_type
	uint16_t screen_functions; // SSD, UC etc.
	uint8_t screen_w_h_inversed_ble;
	uint16_t screen_w_h_inversed;
	uint16_t screen_h;
	uint16_t screen_w;
	uint16_t screen_h_offset;
	uint16_t screen_w_offset;
	uint8_t screen_colors;
	uint8_t screen_color_black_invert;
	uint8_t screen_color_second_invert;
	uint8_t oepl_enabled;
	uint32_t oepl_wakeup_time; // default 40 seconds
	uint8_t ble_enabled;
	uint32_t ble_wakeup_time; // default 3200

	uint8_t nfc_available; //0 = no other = type
	uint8_t button_available; //0 = no other = amount
	uint8_t led_available; //0 = no other = amount

	uint8_t mac_address[8];
	uint8_t ble_mac_address[6];
	uint8_t ble_device_name[40];
	uint8_t ble_device_name_len;

	uint32_t saved_EEPROM_IMG_EACH;

	uint8_t crc;// Needs to be at the last position otherwise the settings can not be validated on next boot!!!!
} settings_struct;
typedef struct Settings_default_type_struct
{
	uint32_t magic;
	uint32_t version;
	uint32_t len;

	uint16_t oepl_hw_type;

	uint8_t crc;// Needs to be at the last position otherwise the settings can not be validated on next boot!!!!
} settings_default_type_struct;

/// DEFAULT SETTINGS FOR DIFFERENT HARDWARE
typedef enum {
    DEVICE_TYPE_BARE_SOC = 0,
	DEVICE_TYPE_YA_BWR_154_H_SSD,
    MAX_LEN_TYPE
} DeviceType;

#define DEFAULT_DEVICE_TYPE DEVICE_TYPE_YA_BWR_154_H_SSD

void init_settings(void);
void settings_set_screen_data(uint16_t screen_type_in);
void reset_settings_to_default(uint16_t screen_type_in);
void set_custom_mac(uint8_t *newMac);
void save_settings_to_flash(void);
uint8_t get_crc_setting(void);
uint8_t get_crc_default(void);

extern settings_struct settings;
