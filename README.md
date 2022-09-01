# lv_port_stm32h7b3i_disco

This repository contains the Light and Versatile Graphics Library (LVGL) port for the STM32H7B3I-DK development board. 

This project uses LVGL v8.3.1

For the most up-to-date version of LVGL please visit: https://github.com/lvgl/lvgl


## How to import this project to STM32CUBEIDE:
1. Clone this respository: 
```
git clone https://github.com/epichl25/lv_port_stm32h7b3i_disco.git
```
2. Open STM32CUBEIDE
3. Create a new workspace
4. Import the project: File => Import => General => Existing Projects into Workspace => Browse => Select the "lv_port_stm32h7b3i_disco" folder => Finish
5. Build the project: Project => Build all
6. Upload the firmware to the development board: Run


## Update Information

### 26.8.2022
DMA2D support was added to the project.

This method uses the Chrom-Art Accelerator found in STM32H7B3I-DK's MCU for rendering. Compared to the "simple" mode, this mode significantly improves performance and reduces stuttering.

### 2.9.2022
1. The project defaults to using the "simple" mode to render on the display. Using STM32's DMA2D is now possible by enabling the define "LV_USE_GPU_STM32_DMA2D" found in lv_conf.h.

2. The project was re-formatted to confrom with LVGL's project format for stm32 projects.

## Benchmark Results

The benchmark demo firmware found in LVGL's demos was used to benchmark performance.

| Rendering method | simple | dma2d |
| :---             |  :---: | :---: |
| Weighted FPS     |   40   |  76   |
| Opa. speed       |   92%  |  83%  |


## How to change rendering mode from default to DMA2D mode
NOTE: The simple rendering method is set by default. For better performance, please enable DMA2D mode.
1. Navigate to "lv_conf.h" 
2. Locate the define "LV_USE_GPU_STM32_DMA2D" found under GPU.
3. Change '0' to '1' to enable DMA2D.  

## TODO
1. Add a demo pic
2. Add a troubleshooting guide
3. Add support for SDRAM buffers and test performance.

