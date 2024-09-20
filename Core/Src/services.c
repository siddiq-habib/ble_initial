#include "app_bluenrg.h"
#include "services.h"
#include "bluenrg_conf.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "stdio.h"
#include "stdlib.h"

const uint8_t SERVICE_UUID[16] 	= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
const uint8_t CHAR_UUID[16]  	= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
uint16_t my_service_handle, my_char_handle;


tBleStatus add_simple_service(void)
	{
	Service_UUID_t service_uuid;
	Char_UUID_t char_uuid;
	tBleStatus ret;
	BLUENRG_memcpy(service_uuid.Service_UUID_128,SERVICE_UUID,16);
	//add service
	ret=aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 7, &my_service_handle);
	BLUENRG_memcpy(char_uuid.Char_UUID_128,CHAR_UUID,16);

	//add char
	aci_gatt_add_char(my_service_handle,UUID_TYPE_128,&char_uuid,1,CHAR_PROP_NOTIFY,ATTR_PERMISSION_NONE,0,16,0,&my_char_handle);


	return ret;
	}
