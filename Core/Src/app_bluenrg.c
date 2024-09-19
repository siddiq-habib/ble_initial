/*
 * app_bluenrg.c
 *
 *  Created on: Sep 19, 2024
 *      Author: siddi
 */

#include "app_bluenrg.h"
#include "bluenrg_conf.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include <stdio.h>
#include <stdlib.h>
#include "hci.h"

#define bdaddre_size 6
uint8_t SERVER_BDARR[]={0x01,0x02,0x03,0x04,0x05,0x6};
void bluenrg_init(void)
{
	const char *name ="Sidd";
	tBleStatus ret;
	uint8_t bdaddre[bdaddre_size];
	BLUENRG_memcpy(bdaddre,SERVER_BDARR,sizeof(SERVER_BDARR));
	uint16_t service_handle, dev_name_char_handle,appearance_char_handle;
	/*
	 * 1.Initialize HCI
	 * 2.Reset HCI
	 * 3.Configure Device address
	 * 4.Initialize GATT server
	 * 5.Initialize GAP service
	 * 6.Update characteristics
	 * 7.Add custom service
	 * */
	hci_init(0,0);
	hci_reset();
	HAL_Delay(100);
	ret=aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,CONFIG_DATA_PUBADDR_LEN, bdaddre);
	if(ret !=BLE_STATUS_SUCCESS){printf("aci_hal_write_config_data failed \r\n");}
	ret=aci_gatt_init();
	if(ret !=BLE_STATUS_SUCCESS){printf("aci_gatt_init failed \r\n");}
	ret=aci_gap_init(GAP_PERIPHERAL_ROLE,0,0x07,&service_handle,&dev_name_char_handle,&appearance_char_handle);
	if(ret !=BLE_STATUS_SUCCESS){printf("aci_gap_init failed \r\n");}
	ret=aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, strlen(name), (uint8_t*)name);
	if(ret !=BLE_STATUS_SUCCESS){printf("aci_gatt_update_char_value failed \r\n");}
}



void bluenrg_process(void)
{
	tBleStatus ret;
	uint8_t local_name[]={AD_TYPE_COMPLETE_LOCAL_NAME,'B','L','E','-','G','-','U','P'};
	ret=aci_gap_set_discoverable(ADV_IND,
								0, 0,
								PUBLIC_ADDR,
								NO_WHITE_LIST_USE,
								sizeof(local_name),
								local_name,
								0, NULL, 0, 0);

	if(ret !=BLE_STATUS_SUCCESS){printf("aci_gap_set_discoverable failed \r\n");}
}
