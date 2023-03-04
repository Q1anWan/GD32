#ifndef USB_GD32_CORE_H
#define USB_GD32_CORE_H


#include "main.h"
#include "usbd_core.h"
#include "usbd_enum.h"
#include "usb_hid.h"
#include "usbd_transc.h"



#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

#define CUSTOM_HID_INTERFACE               0U

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
//#define USB_DEVICE_LOW_PWR_MODE_SUPPORT

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* endpoint count used by the CDC ACM device */
#define CUSTOMHID_IN_EP                    EP_IN(1U)
#define CUSTOMHID_OUT_EP                   EP_OUT(1U)

#define CUSTOMHID_IN_PACKET                2U
#define CUSTOMHID_OUT_PACKET               2U

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0xA0U)

/* CDC data Tx buffer address offset */
#define HID_TX_ADDR                        (0x140U)

/* CDC data Rx buffer address offset */
#define HID_RX_ADDR                        (0x100U)

/* endpoint count used by the CDC ACM device */
#define EP_COUNT                           (4U)

#define USB_STRING_COUNT                   4U

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)

#define USB_PULLUP                         GPIOG
#define USB_PULLUP_PIN                     GPIO_PIN_8
#define RCU_AHBPeriph_GPIO_PULLUP          RCU_GPIOG

/***@brief***/
#define DESC_LEN_REPORT             96U
#define DESC_LEN_CONFIG             41U

#define MAX_PERIPH_NUM              4U
#ifdef __cplusplus
class cUSBHID
{
	protected:
	
	typedef struct
	{
    uint8_t data[2];

    uint8_t reportID;
    uint8_t idlestate;
    uint8_t protocol;
	} usb_hid_handler;
	
	typedef struct HIDFopHandler 
	{
    void (*periph_config[MAX_PERIPH_NUM])(void);
	} usb_hid_fop_handler;
	
	usb_desc_dev 						HIDDeviceDesc={0};
	
	usb_desc_LANGID  LanguageIdDesc;
	usb_desc_str 		ManufaStr;
	usb_desc_str 		ProductStr;
	usb_desc_str 		SerialStr;

	
	uint8_t* HidStrs[4]={0};
	
	usb_desc		HIDDesc;

	public:
	usb_class HIDFunClass;
	usb_hid_desc_config_set   HIDConfigDesc;
	usb_dev 									HIDUDev;
	usb_hid_handler 					HIDHandler;
	usb_hid_fop_handler 			FopHandler;
	
	uint8_t *HIDReportDescriptor;
	/* USER function declarations */
	/*Config Init*/
	virtual bool Config(uint8_t *ReportDescriptor);
	virtual bool Init(void);
	/* send custom HID report */
	bool Report_send (uint8_t *report, uint16_t len);
	
	/* System function declarations */
	
	uint8_t (*pHIDInit)        (usb_dev *udev, uint8_t config_index);
	uint8_t (*pHIDDeinit)      (usb_dev *udev, uint8_t config_index);
	uint8_t (*pHIDReqHandler)  (usb_dev *udev, usb_req *req);
	void (*pHIDDataIn)         (usb_dev *udev, uint8_t ep_num);
	void (*pHIDDataOut)        (usb_dev *udev, uint8_t ep_num);
};


extern "C" {

}
#endif
#endif