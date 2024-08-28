/**
 * @file tft.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "lvgl/lvgl.h"
#include <string.h>
#include <stdlib.h>

#include "tft.h"
#include "stm32h7xx.h"
#include "stm32h7b3i_discovery.h"
#include "stm32h7b3i_discovery_lcd.h"
#include "stm32h7b3i_discovery_sdram.h"
#include "stm32h7b3i_discovery_ts.h"
#include "../Components/rk043fn48h/rk043fn48h.h"
/*********************
 *      DEFINES
 *********************/
#if LV_COLOR_DEPTH != 16 && LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
#error LV_COLOR_DEPTH must be 16, 24, or 32
#endif

#define TFT_FULL_REFRESH	0

/**********************
 *      TYPEDEFS
 **********************/
#if LV_USE_GPU_STM32_DMA2D
__IO uint32_t   transferCompleteDetected = 0;  /* DMA2D Transfer Complete flag */
HAL_StatusTypeDef HAL_Status = HAL_OK;
#endif
/**********************
 *  STATIC PROTOTYPES
 **********************/
/*These 3 functions are needed by LittlevGL*/
static void ex_disp_flush(lv_display_t *display, const lv_area_t *area, uint8_t * color_p);

#if LV_USE_GPU_STM32_DMA2D
static void DMA2D_Config(uint32_t xSize);
static void TransferComplete(DMA2D_HandleTypeDef *hlcd_dma2d);
static void TransferError(DMA2D_HandleTypeDef *hlcd_dma2d);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_COLOR_DEPTH == 16
typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
typedef uint32_t uintpixel_t;
#endif


/**********************
 *      MACROS
 **********************/

/**
 * Initialize your display here
 */

void tft_init(void)
{
    BSP_LCD_Init(0,LCD_ORIENTATION_LANDSCAPE);
	BSP_LCD_DisplayOn(0);

	lv_display_t * disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
#if TFT_FULL_REFRESH
	static uint8_t fb[2][TFT_HOR_RES*TFT_VER_RES * 2];
	lv_disp_draw_buf_init(&disp_buf_1, fb[0], fb[1], TFT_HOR_RES * TFT_VER_RES);   /*Initialize the display buffer*/
#else
	static uint8_t buf[TFT_HOR_RES * 68 * 2];
	lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

	lv_display_set_flush_cb(disp, ex_disp_flush);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ex_disp_flush(lv_display_t * disp, const lv_area_t *area, uint8_t * color_p)
{
#if TFT_FULL_REFRESH == 0
	uint16_t * fb = (uint16_t *) LCD_LAYER_0_ADDRESS;
	fb += area->y1 * TFT_HOR_RES;
	fb += area->x1;
	int32_t w = lv_area_get_width(area) * 2;
	int32_t y;
	for(y = area->y1; y <= area->y2; y++) {
		lv_memcpy(fb, color_p, w);
		fb += TFT_HOR_RES;
		color_p += w;
	}
#else
	HAL_LTDC_SetAddress(&hlcd_ltdc, color_p, 0);
#endif
	lv_display_flush_ready(disp);
}
