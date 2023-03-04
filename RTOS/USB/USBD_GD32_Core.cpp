#include "USBD_GD32_Core.h"
#include <string.h>

#define USBD_VID                     0x28E9U
#define USBD_PID                     0x128AU

/* Note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
bool cUSBHID::Config(uint8_t *ReportDescriptor)
{
	/* USB standard device descriptor */
	usb_desc_dev tmp_hid_dev_desc =
	{
			.header = 
			 {
					 .bLength          = USB_DEV_DESC_LEN, 
					 .bDescriptorType  = USB_DESCTYPE_DEV,
			 },
			.bcdUSB                = 0x0200U,
			.bDeviceClass          = 0x00U,
			.bDeviceSubClass       = 0x00U,
			.bDeviceProtocol       = 0x00U,
			.bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
			.idVendor              = USBD_VID,
			.idProduct             = USBD_PID,
			.bcdDevice             = 0x0100U,
			.iManufacturer         = STR_IDX_MFC,
			.iProduct              = STR_IDX_PRODUCT,
			.iSerialNumber         = STR_IDX_SERIAL,
			.bNumberConfigurations = USBD_CFG_MAX_NUM,
	};
	this->HIDDeviceDesc = tmp_hid_dev_desc;
	
	/* USB device configure descriptor */
	usb_hid_desc_config_set tmp_hid_config_desc = 
	{
			.config = 
			{
					.header = 
					 {
							 .bLength         = sizeof(usb_desc_config), 
							 .bDescriptorType = USB_DESCTYPE_CONFIG 
					 },
					.wTotalLength         = DESC_LEN_CONFIG,
					.bNumInterfaces       = 0x01U,
					.bConfigurationValue  = 0x01U,
					.iConfiguration       = 0x00U,
					.bmAttributes         = 0x80U,
					.bMaxPower            = 0x32U
			},

			.hid_itf = 
			{
					.header = 
					 {
							 .bLength         = sizeof(usb_desc_itf), 
							 .bDescriptorType = USB_DESCTYPE_ITF 
					 },
					.bInterfaceNumber     = 0x00U,
					.bAlternateSetting    = 0x00U,
					.bNumEndpoints        = 0x02U,
					.bInterfaceClass      = USB_HID_CLASS,
					.bInterfaceSubClass   = 0x00U,
					.bInterfaceProtocol   = 0x00U,
					.iInterface           = 0x00U
			},

			.hid_vendor = 
			{
					.header = 
					 {
							 .bLength         = sizeof(usb_desc_hid),
							 .bDescriptorType = USB_DESCTYPE_HID 
					 },
					.bcdHID               = 0x0111U,
					.bCountryCode         = 0x00U,
					.bNumDescriptors      = 0x01U,
					.bDescriptorType      = USB_DESCTYPE_REPORT,
					.wDescriptorLength    = DESC_LEN_REPORT,
			},

			.hid_epin = 
			{
					.header = 
					 {
							 .bLength         = sizeof(usb_desc_ep), 
							 .bDescriptorType = USB_DESCTYPE_EP 
					 },
					.bEndpointAddress     = CUSTOMHID_IN_EP,
					.bmAttributes         = USB_EP_ATTR_INT,
					.wMaxPacketSize       = CUSTOMHID_IN_PACKET,
					.bInterval            = 0x20U
			},

			.hid_epout = 
			{
					.header = 
					 {
							 .bLength         = sizeof(usb_desc_ep), 
							 .bDescriptorType = USB_DESCTYPE_EP
					 },
					.bEndpointAddress     = CUSTOMHID_OUT_EP,
					.bmAttributes         = USB_EP_ATTR_INT,
					.wMaxPacketSize       = CUSTOMHID_OUT_PACKET,
					.bInterval            = 0x20U
			}
	};
	this->HIDConfigDesc = tmp_hid_config_desc;
	
	/* USB language ID descriptor */
	usb_desc_LANGID tmp_language_id_desc = 
	{
			.header = 
			 {
					 .bLength = sizeof(usb_desc_LANGID), 
					 .bDescriptorType = USB_DESCTYPE_STR
			 },
			.wLANGID = ENG_LANGID
	};
	this->LanguageIdDesc = tmp_language_id_desc;
	
	/* USB manufacture string */
	usb_desc_str tmp_manufacturer_string = 
	{
			.header = 
			 {
					 .bLength         = USB_STRING_LEN(10U), 
					 .bDescriptorType = USB_DESCTYPE_STR,
			 },
			.unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
	};
	this->ManufaStr = tmp_manufacturer_string;
	
	/* USB product string */
	usb_desc_str tmp_product_string = 
	{
			.header = 
			 {
					 .bLength         = USB_STRING_LEN(14U), 
					 .bDescriptorType = USB_DESCTYPE_STR,
			 },
			.unicode_string = {'G', 'D', '3', '2', '-', 'C', 'u', 's', 't', 'o', 'm', 'H', 'I', 'D'}
	};
	this->ProductStr = tmp_product_string;
	
	/* USB serial string */
	usb_desc_str tmp_serial_string = 
	{
			.header = 
			 {
					 .bLength         = USB_STRING_LEN(12U), 
					 .bDescriptorType = USB_DESCTYPE_STR,
			 }
	};
	this->SerialStr = tmp_serial_string;
	
	/* USB string descriptor set */
	HidStrs[STR_IDX_LANGID] = (uint8_t *)&this->LanguageIdDesc;
	HidStrs[STR_IDX_MFC] = (uint8_t *)&this->ManufaStr;
	HidStrs[STR_IDX_PRODUCT] = (uint8_t *)&this->ProductStr;
	HidStrs[STR_IDX_SERIAL] = (uint8_t *)&this->SerialStr;

	usb_desc tmp_hid_desc = 
	{
			.dev_desc    = (uint8_t *)&this->HIDDeviceDesc,
			.config_desc = (uint8_t *)&this->HIDConfigDesc,
			.strings     = this->HidStrs
	};	 
	this->HIDDesc = tmp_hid_desc;
	

	
	usb_class tmp_hid_class = {
	.req_cmd       = 0xFFU,
		
	.init          = this->pHIDInit,
	.deinit        = this->pHIDDeinit,
	.req_process   = this->pHIDReqHandler,
	.data_in       = this->pHIDDataIn,
	.data_out      = this->pHIDDataOut
	};
	
	this->HIDFunClass 	= tmp_hid_class;
	
	/***@brief      register HID interface operation functions*/
	HIDUDev.user_data		= &FopHandler;
	/***@brief      Link Function*/
	this->HIDReportDescriptor = ReportDescriptor;
	
	return USBD_OK;
}	
bool cUSBHID::Init(void)
{

	return 0;
}    
uint8_t cUSBHID::HIDInit(usb_dev *udev, uint8_t config_index)
{
 /* initialize the data endpoints */
	
	usbd_ep_init(&HIDUDev, EP_BUF_SNG, HID_TX_ADDR, &HIDConfigDesc.hid_epin);
	usbd_ep_init(&HIDUDev, EP_BUF_SNG, HID_RX_ADDR, &HIDConfigDesc.hid_epout);

	usbd_ep_recev (&HIDUDev, CUSTOMHID_OUT_EP, HIDHandler.data, 2U);

	HIDUDev.ep_transc[EP_ID(CUSTOMHID_IN_EP)][TRANSC_IN] 	= this->HIDDataIn;
	HIDUDev.ep_transc[EP_ID(CUSTOMHID_OUT_EP)][TRANSC_OUT] = HIDDataOut;

	HIDUDev.class_data[CUSTOM_HID_INTERFACE] = (void *)&HIDHandler;

	if (HIDUDev.user_data != NULL) {
			for (uint8_t i = 0U; i < MAX_PERIPH_NUM; i++) {
					if (((usb_hid_fop_handler *)HIDUDev.user_data)->periph_config[i] != NULL) {
							((usb_hid_fop_handler *)HIDUDev.user_data)->periph_config[i]();
					}
			}
	}

	return USBD_OK;
}

/*!
    \brief      de-initialize the HID device
    \param[in]  HIDUDev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cUSBHID::HIDDeinit(usb_dev *udev, uint8_t config_index)
{
    /* deinitialize HID endpoints */
    usbd_ep_deinit(&HIDUDev, CUSTOMHID_IN_EP);
    usbd_ep_deinit(&HIDUDev, CUSTOMHID_OUT_EP);

    return USBD_OK;
}

const uint8_t customhid_report_descriptor[DESC_LEN_REPORT] =
{
    0x06, 0x00, 0xFF,  /* USAGE_PAGE (Vendor Defined: 0xFF00) */
    0x09, 0x00,        /* USAGE (Custom Device)               */
    0xa1, 0x01,        /* COLLECTION (Application)            */

    /* led 1 */
    0x85, 0x11,     /* REPORT_ID (0x11)          */
    0x09, 0x01,     /* USAGE (LED 1)             */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)       */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1)       */
    0x75, 0x08,     /* REPORT_SIZE (8)           */
    0x95, 0x01,     /* REPORT_COUNT (1)          */
    0x91, 0x82,     /* OUTPUT (Data,Var,Abs,Vol) */

    /* led 2 */
    0x85, 0x12,     /* REPORT_ID (0x12)          */
    0x09, 0x02,     /* USAGE (LED 2)             */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)       */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1)       */
    0x75, 0x08,     /* REPORT_SIZE (8)           */
    0x95, 0x01,     /* REPORT_COUNT (1)          */
    0x91, 0x82,     /* OUTPUT (Data,Var,Abs,Vol) */

    /* led 3 */
    0x85, 0x13,     /* REPORT_ID (0x13)          */
    0x09, 0x03,     /* USAGE (LED 3)             */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)       */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1)       */
    0x75, 0x08,     /* REPORT_SIZE (8)           */
    0x95, 0x01,     /* REPORT_COUNT (1)          */
    0x91, 0x82,     /* OUTPUT (Data,Var,Abs,Vol) */

    /* led 4 */
    0x85, 0x14,     /* REPORT_ID (0x14)          */
    0x09, 0x04,     /* USAGE (LED 4)             */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)       */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1)       */
    0x75, 0x08,     /* REPORT_SIZE (8)           */
    0x95, 0x01,     /* REPORT_COUNT (1)          */
    0x91, 0x82,     /* OUTPUT (Data,Var,Abs,Vol) */

    /* wakeup key */
    0x85, 0x15,     /* REPORT_ID (0x15)          */
    0x09, 0x05,     /* USAGE (Push Button)       */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)       */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1)       */
    0x75, 0x01,     /* REPORT_SIZE (1)           */
    0x81, 0x02,     /* INPUT (Data,Var,Abs,Vol)  */

    0x75, 0x07,     /* REPORT_SIZE (7)           */
    0x81, 0x03,     /* INPUT (Cnst,Var,Abs,Vol)  */

    /* tamper key */
    0x85, 0x16,     /* REPORT_ID (0x16)          */
    0x09, 0x06,     /* USAGE (Push Button)       */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)       */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1)       */
    0x75, 0x01,     /* REPORT_SIZE (1)           */
    0x81, 0x02,     /* INPUT (Data,Var,Abs,Vol)  */

    0x75, 0x07,     /* REPORT_SIZE (7)           */
    0x81, 0x03,     /* INPUT (Cnst,Var,Abs,Vol)  */

    0xc0            /* END_COLLECTION            */
};


/*!
    \brief      send custom HID report
    \param[in]  HIDUDev: pointer to USB device instance
    \param[in]  report: pointer to HID report
    \param[in]  len: data length
    \param[out] none
    \retval     USB device operation status
*/
bool cUSBHID::Report_send (uint8_t *report, uint16_t len)
{
	usbd_ep_send (&HIDUDev, CUSTOMHID_IN_EP, report, len);
	return USBD_OK;
}


/*!
    \brief      handle the HID class-specific requests
    \param[in]  HIDUDev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cUSBHID::HIDReqHandler (usb_dev *HIDUDev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP;

    usb_hid_handler *hid = (usb_hid_handler *)HIDUDev->class_data[CUSTOM_HID_INTERFACE];

    switch (req->bRequest) {
    case USB_GET_DESCRIPTOR:
        if (USB_DESCTYPE_REPORT == (req->wValue >> 8)) {
            usb_transc_config(&HIDUDev->transc_in[0], 
                              (uint8_t *)customhid_report_descriptor, 
                              USB_MIN(DESC_LEN_REPORT, req->wLength), 
                              0U);

            status = REQ_SUPP;
        } else if (USB_DESCTYPE_HID == (req->wValue >> 8U)) {
            usb_transc_config(&HIDUDev->transc_in[0U], 
                              (uint8_t *)(&(HIDConfigDesc.hid_vendor)), 
                              USB_MIN(9U, req->wLength), 
                              0U);
        }
        break;

    case GET_REPORT:
        if (2U == req->wLength) {
            usb_transc_config(&HIDUDev->transc_in[0], hid->data, 2U, 0U);

            status = REQ_SUPP;
        }
        break;

    case GET_IDLE:
        usb_transc_config(&HIDUDev->transc_in[0], (uint8_t *)&hid->idlestate, 1U, 0U);

        status = REQ_SUPP;
        break;

    case GET_PROTOCOL:
        usb_transc_config(&HIDUDev->transc_in[0], (uint8_t *)&hid->protocol, 1U, 0U);

        status = REQ_SUPP;
        break;

    case SET_REPORT:
        hid->reportID = (uint8_t)(req->wValue);

        usb_transc_config(&HIDUDev->transc_out[0], hid->data, req->wLength, 0U);

        status = REQ_SUPP;
        break;

    case SET_IDLE:
        hid->idlestate = (uint8_t)(req->wValue >> 8);

        status = REQ_SUPP;
        break;

    case SET_PROTOCOL:
        hid->protocol = (uint8_t)(req->wValue);

        status = REQ_SUPP;
        break;

    default:
        break;
    }

    return status;
}

/*!
    \brief      handle custom HID data in transaction
    \param[in]  HIDUDev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
void cUSBHID::HIDDataIn (usb_dev *udev, uint8_t ep_num)
{
    return;
}

/*!
    \brief      handle custom HID data out transaction
    \param[in]  HIDUDev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
void cUSBHID::HIDDataOut (usb_dev *udev, uint8_t ep_num)
{
    usb_hid_handler *hid = (usb_hid_handler *)HIDUDev.class_data[CUSTOM_HID_INTERFACE];

    if (CUSTOMHID_OUT_EP == ep_num){
        switch (hid->data[0]){
        case 0x11:
            if (RESET != hid->data[1]) {
               
               
            } else {
                
            }
            break;
        case 0x12:
            if (RESET != hid->data[1]) {
                
            } else {
               
            }
            break;
        default:
            break;
        }

        usbd_ep_recev(&HIDUDev, CUSTOMHID_OUT_EP, hid->data, 2U);
    }
}
