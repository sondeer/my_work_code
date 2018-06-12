/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @author         : MCD Application Team
  * @version        : V2.2.0
  * @date           : 13-June-2014
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"
#include "usb_device.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 1 */
/* USAGE_PAGE                              */ 0x06u, 0x00u, 0xFFu,
/* USAGE                                   */ 0x09u, 0x01u,
/* COLLECTION                              */ 0xA1u, 0x01u,
/* USAGE_MINIMUM                           */ 0x19u, 0x00u,
/* USAGE_MAXIMUM                           */ 0x29u, 0x40u,
/* LOGICAL_MINIMUM                         */ 0x15u, 0x00u,
/* LOGICAL_MAXIMUM                         */ 0x26u, 0xFFu, 0x00u,
/* REPORT_SIZE                             */ 0x75u, 0x08u,
/* REPORT_COUNT                            */ 0x95u, 0x40u,
/* OUTPUT                                  */ 0x91u, 0x00u,
/* USAGE_MINIMUM                           */ 0x19u, 0x00u,
/* USAGE_MAXIMUM                           */ 0x29u, 0x40u,
/* LOGICAL_MINIMUM                         */ 0x15u, 0x00u,
/* LOGICAL_MAXIMUM                         */ 0x26u, 0xFFu, 0x00u,
/* REPORT_SIZE                             */ 0x75u, 0x08u,
/* REPORT_COUNT                            */ 0x95u, 0x40u,
/* INPUT                                   */ 0x81u, 0x00u,
/* END_COLLECTION                          */ 0xC0u,
  /* USER CODE END 1 */
};
/* USB handler declaration */
/* Handle for USB High Speed IP */
USBD_HandleTypeDef  *hUsbDevice_1;
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static int8_t CUSTOM_HID_Init_FS     (void);
static int8_t CUSTOM_HID_DeInit_FS   (void);
static int8_t CUSTOM_HID_OutEvent_FS (void *buff, int  size);
/* USER CODE BEGIN 3 */
/* USER CODE END 3 */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS,
};

/**
  * @brief  CUSTOM_HID_Init_FS
  *         Initializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
   hUsbDevice_1 = &hUsbDeviceFS;
  /* USER CODE BEGIN 8 */
  return (0);
  /* USER CODE END 8 */
}

/**
  * @brief  CUSTOM_HID_DeInit_FS
  *         DeInitializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 9 */
  return (0);
  /* USER CODE END 9 */
}



/**
  * @brief  CUSTOM_HID_OutEvent_FS
  *         Manage the CUSTOM HID class events
  * @param  event_idx: event index
  * @param  state: event state
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS (void *buff, int  size)
{
  /* USER CODE BEGIN 10 */
	  UsbReceivData(buff,size);
    return (0);
  /* USER CODE END 10 */
}

/* USER CODE BEGIN 11 */
/**
  * @brief  USBD_CUSTOM_HID_SendReport_FS
  *         Send the report to the Host
  * @param  report: the report to be sent
  * @param  len: the report length
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS ( uint8_t *report,uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(hUsbDevice_1, report, len);
}
*/
/* USER CODE END 11 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
