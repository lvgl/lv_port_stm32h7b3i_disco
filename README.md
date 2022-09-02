# lv_port_stm32h7b3i_disco

This repository contains the Light and Versatile Graphics Library (LVGL) port for the STM32H7B3I-DK development board. 

![demo_pic](/pics/demo_pic.jpg)

This project uses LVGL v8.3.1

For the most up-to-date version of LVGL please visit: https://github.com/lvgl/lvgl

The STM32H7B3I-DK is an affordable development board with:
- STM32H7B3LIH6QU Arm®-based microcontroller featuring 2 Mbytes of Flash memory and 1.4 Mbyte of RAM in BGA225 package 
- 4.3" (480x272 pixels) TFT color LCD module including a capacitive touch panel with RGB interface 
- Wi‑Fi® module compliant with 802.11 b/g/n 
- USB OTG HS 
- Audio codec 
- 512-Mbit Octo-SPI NOR Flash memory 
- 128-Mbit SDRAM 
- 2 user LEDs 
- User and Reset push-buttons 
- Fanout daughterboard 
- 1x FDCAN 
- Camera (8 bit) 
- USB with Micro-AB 
- Stereo headset jack including analog microphone input 
- Audio jack for external speakers 
- microSD™ card 
- TAG-Connect 10-pin footprint 
- Arm® Cortex® 10-pin 1.27mm-pitch debug connector over STDC14 footprint 
- ARDUINO® Uno V3 expansion connector 
- STMod+ expansion connector 
- Audio daughterboard expansion connector 
- External I2C expansion connector 

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
1. The project defaults to using the "simple" mode to render. Using STM32's DMA2D must now be enabled by changing the value of the define "LV_USE_GPU_STM32_DMA2D" found in lv_conf.h.

2. The project was re-formatted to confrom with LVGL's project structure for stm32 projects.

## Benchmark Results
The benchmark demo firmware found in LVGL's demos was used to benchmark performance.

| Rendering method | simple | dma2d |
| :---             |  :---: | :---: |
| Weighted FPS     |   40   |  76   |
| Opa. speed       |   92%  |  83%  |


## How to change rendering mode from default to DMA2D mode
NOTE: The simple rendering method is set by default. For better performance, please enable DMA2D mode.
1. Navigate to "lv_conf.h" 
2. Locate the define "LV_USE_GPU_STM32_DMA2D" found in the GPU section.
3. Change '0' to '1' to enable DMA2D.  

## Troubleshooting
Below lists some common problems users might face when using this project.

1. The project fails to build due to include errors.

- Solution 1 
<br />Check the workspace is named "lv_port_stm32h7b3i_disco". If it is not, rename it by right-clicking on the workspace and selecting rename. 

- Solution 2
1. If you do not wish to rename the workspace, go to "Project=>Properties=>C/C++ General=>Paths and Symbols=>Includes=>GNU C=>Include directories".
2. Remove the included directory "/lv_port_stm32h7b3i_disco".
3. Add a new inlcude directory "Add=>Workspace" and click on the workspace.
4. Ensure "Is a workspace path" is checked and press "OK".
5. Build and run the project again.

## TODO
Add support for SDRAM buffers and test performance.

## Credits

- epichl25
<br />Original creator of this repository.
<br />GitHub page link: https://github.com/epichl25

- kisvegabor
<br />Creator of LVGL.
<br />GitHub page link: https://github.com/kisvegabor
