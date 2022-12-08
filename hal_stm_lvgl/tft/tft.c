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
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p);
static void ex_disp_clean_dcache(lv_disp_drv_t *drv);

#if LV_USE_GPU_STM32_DMA2D
static void DMA2D_Config(uint32_t xSize);
static void TransferComplete(DMA2D_HandleTypeDef *hlcd_dma2d);
static void TransferError(DMA2D_HandleTypeDef *hlcd_dma2d);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t disp_drv;

#if LV_COLOR_DEPTH == 16
typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
typedef uint32_t uintpixel_t;
#endif

static lv_disp_t *our_disp = NULL;

#if LV_USE_GPU_STM32_DMA2D
static int32_t            x1_flush;
static int32_t            y1_flush;
static int32_t            x2_flush;
static int32_t            y2_fill;
static int32_t            y_fill_act;
static const lv_color_t * buf_to_flush;
#endif

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

   /* LVGL requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row*/

	static lv_disp_draw_buf_t disp_buf_1;
#if TFT_FULL_REFRESH
	static lv_color_t fb[2][TFT_HOR_RES*TFT_VER_RES];
	lv_disp_draw_buf_init(&disp_buf_1, fb[0], fb[1], TFT_HOR_RES * TFT_VER_RES);   /*Initialize the display buffer*/
#else
	static lv_color_t buf[TFT_HOR_RES * 68];
	lv_disp_draw_buf_init(&disp_buf_1, buf, NULL, TFT_HOR_RES * 68);   /*Initialize the display buffer*/
#endif

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
	disp_drv.full_refresh = TFT_FULL_REFRESH;

	/*Set a display buffer*/
	disp_drv.draw_buf = &disp_buf_1;

	/*Finally register the driver*/
	our_disp = lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_flush_ready()' has to be called when finished
 * This function is required only when LV_VDB_SIZE != 0 in lv_conf.h*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p)
{
#if TFT_FULL_REFRESH == 0
	uint16_t * fb = (uint16_t *) LCD_LAYER_0_ADDRESS;
	uint16_t stride = disp_drv.hor_res;
	fb += area->y1 * stride;
	fb += area->x1;
	lv_coord_t w = lv_area_get_width(area);
	int32_t y;
	for(y = area->y1; y <= area->y2; y++) {
		lv_memcpy(fb, color_p, w * 2);
		fb += stride;
		color_p += w;
	}
#else
	HAL_LTDC_SetAddress(&hlcd_ltdc, color_p, 0);
#endif
	lv_disp_flush_ready(&disp_drv);
}


static void ex_disp_clean_dcache(lv_disp_drv_t *drv)
{
    SCB_CleanInvalidateDCache();
}

#if LV_USE_GPU_STM32_DMA2D
/**
  * @brief DMA2D configuration.
  * @note  This function Configure the DMA2D peripheral :
  *        1) Configure the transfer mode : register to memory
  *        2) Configure the color to be used to fill user defined area.
  * @retval
  *  None
  */

static void DMA2D_Config(uint32_t xSize)
{

	/* Configure the DMA2D Mode, Color Mode and output offset */
	hlcd_dma2d.Init.Mode = DMA2D_M2M_PFC;
	hlcd_dma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888; /* Output color out of PFC */
	/* Output offset in pixels == nb of pixels to be added at end of line to come to the  */
	/* first pixel of the next line : on the output side of the DMA2D                     */
	hlcd_dma2d.Init.OutputOffset = LCD_DEFAULT_WIDTH - xSize;
	hlcd_dma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR; /* No Output Red & Blue swap */
	hlcd_dma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/

	/*##-2- DMA2D Callbacks Configuration ######################################*/
	hlcd_dma2d.XferCpltCallback  = TransferComplete;
	hlcd_dma2d.XferErrorCallback = TransferError;

	/* Foreground configuration: Layer 1 */
	hlcd_dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hlcd_dma2d.LayerCfg[1].InputAlpha = 0xFF; /* Fully opaque */
	hlcd_dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888; /* Foreground
	layer format is ARGB8888 : 32 bpp */
	hlcd_dma2d.LayerCfg[1].InputOffset = 0x0; /* No offset in input */
	hlcd_dma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No R&B
	swap for the input foreground image */
	hlcd_dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No alpha
	inversion for the input foreground image */

	hlcd_dma2d.Instance = DMA2D;

	/* DMA2D Initialization */
	if (HAL_DMA2D_Init(&hlcd_dma2d) == HAL_OK)
	{
		if (HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1)==HAL_OK) {;}
		else
		{
			while(1) {;}
		}

	}
	else
	{
		while(1) {;}
	}

}

/**
  * @brief  DMA2D Transfer completed callback
  * @param  hdma2d: DMA2D handle.
  * @note   This example shows a simple way to report end of DMA2D transfer, and
  *         you can add your own implementation.
  * @retval None
  */
static void TransferComplete(DMA2D_HandleTypeDef *hlcd_dma2d)
{

	  lv_disp_flush_ready(&disp_drv);
//	transferCompleteDetected = 1;
}

/**
  * @brief  DMA2D error callbacks
  * @param  hdma2d: DMA2D handle
  * @note   This example shows a simple way to report DMA2D transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
static void TransferError(DMA2D_HandleTypeDef *hlcd_dma2d)
{

}
#endif
