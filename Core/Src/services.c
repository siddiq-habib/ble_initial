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


uint8_t  connected=0;
uint8_t  set_connectable=1;
uint16_t connection_handle=0;
uint8_t  notification_enabled=0;

tBleStatus add_simple_service(void)
{
	Service_UUID_t service_uuid;
	Char_UUID_t char_uuid;
	tBleStatus ret;
	BLUENRG_memcpy(service_uuid.Service_UUID_128,SERVICE_UUID,16);
	//add service
	ret=aci_gatt_add_service(UUID_TYPE_128,
			&service_uuid,
			PRIMARY_SERVICE,
			7,
			&my_service_handle);
	BLUENRG_memcpy(char_uuid.Char_UUID_128,CHAR_UUID,16);

	//add char
	ret = aci_gatt_add_char(my_service_handle,UUID_TYPE_128,
						&char_uuid,2,
						CHAR_PROP_READ,
						ATTR_PERMISSION_NONE,GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
						0,0,&my_char_handle);
	return ret;
}


void update_data(int16_t new_data)
{
	tBleStatus ret;
	/*Update characteristics value*/
	ret=aci_gatt_update_char_value(my_service_handle, my_char_handle, 0, 2, (uint8_t *)&new_data);
	if(ret !=BLE_STATUS_SUCCESS){printf("aci_gatt_update_char_value failed \r\n");}
}

void Read_Request_CB(uint16_t handle)
{
	int data;
	data=450+((uint16_t)rand()*100)/1000;
	printf("the hex data to be sent is 0x%.8X\r\n",data);
	update_data(data);
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

