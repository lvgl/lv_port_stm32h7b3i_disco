/**
 * @file indev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "hal_stm_lvgl/tft/tft.h"

#include "stm32h7b3i_discovery.h"
#include "stm32h7b3i_discovery_ts.h"
#include "stm32h7xx.h"
/*********************
 *      DEFINES
 *********************/
#define TS_INSTANCE		(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t *data);

/**********************
 *  STATIC VARIABLES
 **********************/
static TS_State_t  TS_State;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize your input devices here
 */
void touchpad_init(void)
{
	TS_Init_t hTS;
	hTS.Width = TFT_HOR_RES;
	hTS.Height = TFT_VER_RES;
	hTS.Orientation = TS_SWAP_XY;
	hTS.Accuracy = 0;

    BSP_TS_Init(TS_INSTANCE, &hTS);

    lv_indev_t * indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, touchpad_read);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t *data)
{
    /* Read your touchpad */
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    BSP_TS_GetState(TS_INSTANCE, &TS_State);  /*Get touch state*/

    if(TS_State.TouchDetected)
    {
            data->point.x = TS_State.TouchX;
            data->point.y = TS_State.TouchY;
            last_x = data->point.x;
            last_y = data->point.y;
            data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
            data->point.x = last_x;
            data->point.y = last_y;
            data->state = LV_INDEV_STATE_RELEASED;
    }
}
