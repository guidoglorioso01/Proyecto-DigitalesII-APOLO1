/**
  ******************************************************************************
  * File Name          : TouchGFXConfiguration.cpp
  ******************************************************************************
  * This file is generated by TouchGFX Generator 4.22.0. Please, do not edit!
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <texts/TypedTextDatabase.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <BitmapDatabase.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>
#include <STM32DMA.hpp>
#include <TouchGFXHAL.hpp>
#include <STM32TouchController.hpp>
#include <stm32f4xx_hal.h>

extern "C" void touchgfx_init();
extern "C" void touchgfx_taskEntry();
extern "C" void touchgfx_components_init();

static STM32TouchController tc;
static STM32DMA dma;
static LCD16bpp display;
static ApplicationFontProvider fontProvider;
static Texts texts;
static TouchGFXHAL hal(dma, display, tc, 480, 320);

void touchgfx_init()
{
    Bitmap::registerBitmapDatabase(BitmapDatabase::getInstance(), BitmapDatabase::getInstanceSize());
    TypedText::registerTexts(&texts);
    Texts::setLanguage(0);

    FontManager::setFontProvider(&fontProvider);

    FrontendHeap& heap = FrontendHeap::getInstance();
    /*
     * we need to obtain the reference above to initialize the frontend heap.
     */
    (void)heap;

    /*
     * Initialize TouchGFX
     */
    hal.initialize();
}

void touchgfx_components_init()
{
}

void touchgfx_taskEntry()
{
    /*
     * Main event loop. Will wait for VSYNC signal, and then process next frame. Call
     * this function from your GUI task.
     *
     * Note This function never returns
     */
    hal.taskEntry();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
