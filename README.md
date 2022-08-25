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
Two rendering methods are available. 
#### 1. Simple 
This method simply writes pixels one by one using loops.

#### 2. dma2d
This method uses the dma2d hardware accelerator available on the development board to speed up the rendering performance. Compared to the "simple" method, the use of dma2d significantly improves its rendering performance.

## How to change the rendering mode
Navigate to "hal_stm_lvgl/tft/tft.c" and change the defines for simple and dma2d to either "0" or "1". 
Note: You should only enable one of the two.
