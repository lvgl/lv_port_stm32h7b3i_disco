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
#include "stm32h7b3i_discovery_sdram.h"
#include "stm32h7b3i_discovery_ts.h"
#include "../Components/rk043fn48h/rk043fn48h.h"
#include "stm32h7b3i_discovery_lcd.h"
/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH != 16 && LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
#error LV_COLOR_DEPTH must be 16, 24, or 32
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*These 3 functions are needed by LittlevGL*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p);
static void ex_disp_clean_dcache(lv_disp_drv_t *drv);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t disp_drv;

#if LV_COLOR_DEPTH == 16
typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
typedef uint32_t uintpixel_t;
#endif

/* You can try to change buffer to internal ram by uncommenting line below and commenting
 * SDRAM one. */
static uintpixel_t my_fb[TFT_HOR_RES * TFT_VER_RES];
//static __IO uintpixel_t * my_fb = (__IO uintpixel_t*) (0x60000000);

static lv_disp_t *our_disp = NULL;
/**********************
 *      MACROS
 **********************/

/**
 * Initialize your display here
 */

void tft_init(void)
{
	/* There is only one display on STM32 */
	if(our_disp != NULL)
		abort();

    BSP_LCD_Init(0,LCD_ORIENTATION_LANDSCAPE);

    BSP_LCD_DisplayOn(0);

   /*-----------------------------
	* Create a buffer for drawing
	*----------------------------*/

   /* LittlevGL requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row*/

	static lv_disp_draw_buf_t disp_buf_1;
	static lv_color_t buf1_1[TFT_HOR_RES * 68];
	static lv_color_t buf1_2[TFT_HOR_RES * 68];


	lv_disp_draw_buf_init(&disp_buf_1, buf1_1, buf1_2, TFT_HOR_RES * 68);   /*Initialize the display buffer*/
	/*-----------------------------------
	* Register the display in LittlevGL
	*----------------------------------*/

	lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

	/*Set up the functions to access to your display*/

	/*Set the resolution of the display*/
	disp_drv.hor_res = 480;
	disp_drv.ver_res = 272;

	/*Used to copy the buffer's content to the display*/
	disp_drv.flush_cb = ex_disp_flush;
	disp_drv.clean_dcache_cb = ex_disp_clean_dcache;

	/*Set a display buffer*/
	disp_drv.draw_buf = &disp_buf_1;

	/*Finally register the driver*/
	our_disp = lv_disp_drv_register(&disp_drv);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Flush the content of the internal buffer the specific area on the display*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p)
{
		int32_t x;
	    int32_t y;
	    for(y = area->y1; y <= area->y2; y++) {
	        for(x = area->x1; x <= area->x2; x++) {
	        	BSP_LCD_WritePixel(0,x, y, color_p->full);
	            color_p++;
	        }
	    }
	    lv_disp_flush_ready(&disp_drv);
}

static void ex_disp_clean_dcache(lv_disp_drv_t *drv)
{
    SCB_CleanInvalidateDCache();
}
