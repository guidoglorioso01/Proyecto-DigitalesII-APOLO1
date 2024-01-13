/* Functions called by the TouchGFX HAL to invoke the actual data transfer to ILI9341.
 * Pero, 2021
 */

#include <Driver_Display.h>
#include "TouchGFX_DataTransfer.h"

extern void DisplayDriver_TransferCompleteCallback();

static uint8_t isTransmittingData = 0;

uint32_t touchgfxDisplayDriverTransmitActive(void)
{
	return isTransmittingData;
}

void touchgfxDisplayDriverTransmitBlock(uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	isTransmittingData = 1;
	drawRGBBitmap(x,y,pixels, w, h);
	isTransmittingData = 0;
	DisplayDriver_TransferCompleteCallback();
}

