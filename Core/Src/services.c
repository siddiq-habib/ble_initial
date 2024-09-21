#include "app_bluenrg.h"
#include "services.h"
#include "bluenrg_conf.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "stdio.h"
#include "stdlib.h"

/*
 * Number of services: 2
 * Health service and weather service
 *
 * */

/*Health service has 2 characteristics - BPM and the weight of the person*/
/*Weather service has 2 characteristics temperature and humidity  */

const uint8_t HEALTH_SERVICE_UUID[16] 	= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
const uint8_t BPM_CHAR_UUID[16]  		= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
const uint8_t WEIGHT_CHAR_UUID[16]  	= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};



const uint8_t WEATHER_SERVICE_UUID[16] = {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x00,0xf2,0x73,0xd9};
const uint8_t TEMP_CHAR_UUID[16]  		= {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x01,0xf2,0x73,0xd9};
const uint8_t HUM_CHAR_UUID[16]  		= {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x02,0xf2,0x73,0xd9};


uint16_t health_service_handle, bpm_char_handle,weight_char_handle;
uint16_t weather_service_handle, temp_char_handle,hum_char_handle;


uint8_t  connected=0;
uint8_t  set_connectable=1;
uint16_t connection_handle=0;
uint8_t  notification_enabled=0;




tBleStatus add_services(void)
	{
	Service_UUID_t health_service_uuid,weather_service_uuid;
	Char_UUID_t bpm_char_uuid,weight_char_uuid,temp_char_uuid,hum_char_uuid;


	BLUENRG_memcpy(health_service_uuid.Service_UUID_128,HEALTH_SERVICE_UUID,16);
	BLUENRG_memcpy(weather_service_uuid.Service_UUID_128,WEATHER_SERVICE_UUID,16);
	BLUENRG_memcpy(temp_char_uuid.Char_UUID_128,TEMP_CHAR_UUID,16);
	BLUENRG_memcpy(hum_char_uuid.Char_UUID_128,HUM_CHAR_UUID,16);
	BLUENRG_memcpy(bpm_char_uuid.Char_UUID_128,BPM_CHAR_UUID,16);
	BLUENRG_memcpy(weight_char_uuid.Char_UUID_128,WEIGHT_CHAR_UUID,16);

	aci_gatt_add_service(UUID_TYPE_128, &health_service_uuid, PRIMARY_SERVICE, 7, &health_service_handle);
	aci_gatt_add_service(UUID_TYPE_128, &weather_service_uuid, PRIMARY_SERVICE, 7, &weather_service_handle);


	aci_gatt_add_char(health_service_handle,UUID_TYPE_128,&bpm_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_NONE,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&bpm_char_handle);
	aci_gatt_add_char(health_service_handle,UUID_TYPE_128,&weight_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_NONE,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&weight_char_handle);
	aci_gatt_add_char(weather_service_handle,UUID_TYPE_128,&temp_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_NONE,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&temp_char_handle);
	aci_gatt_add_char(weather_service_handle,UUID_TYPE_128,&hum_char_uuid,2,CHAR_PROP_READ,ATTR_PERMISSION_NONE,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&hum_char_handle);

	return 0;
	}



void update_bpm(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(health_service_handle, bpm_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("bpm chatacteristics update failed  failed \r\n");}

	}

void update_weight(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(health_service_handle, weight_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("weight chatacteristics update failed  failed \r\n");}

	}

void update_temp(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(weather_service_handle, temp_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("temp chatacteristics update failed  failed \r\n");}

	}

void update_hum(int16_t new_data)
	{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(weather_service_handle, hum_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("hum chatacteristics update failed  failed \r\n");}

	}

int16_t bpm =85;
int16_t weight=90;
int16_t temp=20;
int16_t hum=80;
uint16_t tempVar = 0;

void Read_Request_CB(uint16_t handle)
{
	if(handle==(bpm_char_handle+1))
	{
		printf("BPM request\r\n");
		bpm += 1;
		tempVar = ((uint16_t)(0xFF00)&(bpm << 8)) | (uint8_t)(bpm >> 8);
		update_bpm(tempVar);
	}

	if(handle==(weight_char_handle+1))
	{
		weight += 2;
		printf("weight request\r\n");
		tempVar = ((uint16_t)(0xFF00)&(weight << 8)) | (uint8_t)(weight >> 8);
		update_weight(tempVar);
	}
	if(handle==(temp_char_handle+1))
	{
		temp +=  3;
		printf("temp request\r\n");
		tempVar = ((uint16_t)(0xFF00)&(temp << 8)) | (uint8_t)(temp >> 8);
		update_temp(tempVar);
	}
	if(handle==(hum_char_handle+1))
	{
		hum+= 4;
		printf("hum request\r\n");
		tempVar = ((uint16_t)(0xFF00)&(hum << 8)) | (uint8_t)(hum >> 8);
		update_hum(tempVar);
	}

	if(connection_handle !=0)
	{
		aci_gatt_allow_read(connection_handle);
	}
}


void aci_gatt_read_permit_req_event(uint16_t Connection_Hanlde,
									uint16_t Atrribute_Handle,
									uint16_t Offset
									)
{
	Read_Request_CB(Atrribute_Handle);
}



void hci_le_connection_complete_event
									(uint8_t Status,
									uint16_t Connection_Handle,
									uint8_t Role,
									uint8_t Peer_Address_Type,
									uint8_t Peer_Address[6],
									uint16_t Conn_Interval,
									uint16_t Conn_Latency,
									uint16_t Supervision_Timeout,
									uint8_t Master_Clock_Accuracy
									)

{
	connected=1;
	connection_handle=Connection_Handle;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	printf("Connected \r\n");
}

void hci_le_disconnection_complete_event
										(uint8_t Status,
										 uint16_t Connection_Handle,
										 uint8_t Reason
										)
{
	connected=0;
	set_connectable=1;
	Connection_Handle=0;
	printf("Disconnected \r\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

void APP_UserEvtRx(void *pData)
{
	uint32_t i;
	hci_spi_pckt *hci_pckt =(hci_spi_pckt *)pData;
	if(hci_pckt->type==HCI_EVENT_PKT)
		{
		//Get data from packet
		hci_event_pckt *event_pckt =(hci_event_pckt*)hci_pckt->data;
		//process meta data
		if(event_pckt->evt ==EVT_LE_META_EVENT)
			{
			//get meta data
			evt_le_meta_event *evt =(void *)event_pckt->data;
			//process each meta data;
			for (i=0;i<(sizeof(hci_le_meta_events_table))/(sizeof(hci_le_meta_events_table_type));i++)
				{
					if(evt->subevent ==hci_le_meta_events_table[i].evt_code)
					{
						hci_le_meta_events_table[i].process((void *)evt->data);
					}
				}
			}
		//process vendor event
		else if(event_pckt->evt==EVT_VENDOR)
				{
				evt_blue_aci *blue_evt= (void *)event_pckt->data;
				for (i=0;i<(sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type));i++)
					{
					if(blue_evt->ecode==hci_vendor_specific_events_table[i].evt_code)
						{
						hci_vendor_specific_events_table[i].process((void*)blue_evt->data);

						}

					}
				}
		else
			{
			for (i=0; i<(sizeof(hci_events_table)/sizeof(hci_events_table_type));i++)
					{

					if(event_pckt->evt==hci_events_table[i].evt_code)
						{
						hci_events_table[i].process((void*)event_pckt->data);
						}

					}
			}
		}

	// process other events


}

