# STM32H7B3I-DK_LVGL

This repository contains the Light and Versatile Graphics Library (LVGL) port for the STM32H7B3I-DK development board. 

This project uses LVGL v8.3.1

For the most up-to-date version of LVGL please visit: https://github.com/lvgl/lvgl


## How to import this project to STM32CUBEIDE:
1. Clone this respository: 
```
git clone https://github.com/epichl25/STM32H7B3I-DK_LVGL.git
```
2. Open STM32CUBEIDE
3. Create a new workspace
4. Import the project: File => Import => General => Existing Projects into Workspace => Browse => Select the "STM32H7B3I-DK_LVGL-main" folder => Finish
5. Build the project: Project => Build all
6. Upload the firmware to the development board: Run


## Update Information

### 26.8.2022

DMA2D support was added to the project.

This method uses the Chrom-Art Accelerator found in STM32H7B3I-DK's MCU for rendering. Compared to the "simple" mode, this mode significantly improves performance and reduces stuttering.


## Benchmark Results

The benchmark demo firmware found in LVGL's demos was used to benchmark the performance of the STM32H8B3I-DK.

| Rendering method | simple | dma2d |
| :---             |  :---: | :---: |
| Weighted FPS     |   40   |  76   |
| Opa. speed       |   92%  |  83%  |


## How to change rendering modes
1. Navigate to "hal_stm_lvgl/tft/tft.c" 
2. Locate the defines "simple" and "dma2d"
3. Enable one of the defines. Do NOT enable both modes simultaneously. 

## TODO
Add support for SDRAM buffers and test performance.

