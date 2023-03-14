//+--------------------------------------------------------------------------
//
// File:        main.cpp
//
// TinyPC - (c) 2023 Plummer's Software LLC.  All Rights Reserved.  
//
//  An ESP32 program that pretends to be a Windows PC by loading and showing
//  a series of images from an SD card
//
// Description:
//
//  Loads files from SD, displays them on built in OLED (M5Stack Core 2)
//
// History:     Mar-14-2023         Davepl      Commented
//
//---------------------------------------------------------------------------

#include <M5Core2.h>
#include <TJpg_Decoder.h>
#include <vector>

// tft_output
//
// Callback function for displaying an image on the screen.
// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    // Stop further decoding as image is running off bottom of screen
    if (y >= M5.Lcd.height())
        return 0;

    // This function will clip the image block rendering automatically at the TFT boundaries
    M5.Lcd.pushImage(x, y, w, h, bitmap);

    // This might work instead if you adapt the sketch to use the Adafruit_GFX library
    // tft.drawRGBBitmap(x, y, bitmap, w, h);

    // Return 1 to decode next block
    return 1;
}

// setup
//
// One-time progrma setup

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\n Testing TJpg_Decoder library");

    // Initialise the TFT
    M5.begin();
    M5.Lcd.setTextColor(0xFFFF, 0x0000);
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setSwapBytes(true); // We need to swap the colour bytes (endianess)
    M5.Lcd.fillScreen(TFT_GREEN);

    // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);

    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);
}

// loop
// 
// Run through a series of images and display them on the TFT, waiting for a screen touch
// in between before advancing

const std::vector<std::string> filenames = 
{ 
    "/bios.jpg", 
    "/desktop.jpg", 
    "/start.jpg", 
    "/desktop.jpg" 
};

void loop()
{
    M5.Lcd.fillScreen(TFT_RED);

    // Time recorded for test purposes
    uint32_t t = millis();

    for (;;)
    {
        for (auto filename: filenames)
        {
            if (JDR_OK != TJpgDec.drawSdJpg(0, 0, filename.c_str()))        // Draw the image
                M5.Lcd.printf("Could not open %s", filename.c_str());

            M5.update();
            while (!(M5.Buttons.event & (E_MOVE | E_RELEASE)))              // Wait for a screen touch
            {
                M5.update();    
                delay(100);
            }
        }
    }
}