#pragma once

#include "timecontrol.h"
#include "max7219.h"
#include "lib8tion/lib8tion.h"

class Display
{
public:
    explicit Display(int clkPin, int dinPin, int csPin)
        : driver(clkPin, dinPin, csPin)
    {}
    Display() = delete;

    void setLowBatteryState(bool value)
    {
        if (lowBattery != value)
        {
            lowBattery = value;
            defaultBrightness = lowBattery ? lowBatteryBrightness : normalModeBrightness;
            driver.MAX7219_SetBrightness(defaultBrightness);
        }
    }

    void flash()
    {
        flashStartMs = millis();
        flashActive = true;
        driver.MAX7219_SetBrightness(INTENSITY_MAX);
    }

    void begin()
    {
        driver.Begin();
        driver.MAX7219_SetBrightness(0x00); // Start with minimum brightness
    }
    
    /// @brief Displays a startup animation on the display.
    /// @return True if the animation is still running, false if it has completed.
    bool drawStartupAnimation()
    {
        static const unsigned long duration = 1500;
        static unsigned long start = millis();
        unsigned long elapsed = millis() - start;
        
        static const char* data = "+*+*+*+*";
        memcpy(contentStaging.characters, data, sizeof(contentStaging.characters));

        float ratio = static_cast<float>(elapsed) / duration;
        ratio = min(ratio, 1.0f);

        char brightness = static_cast<char>(defaultBrightness * ratio);

        // uint8_t ease8 = ease8InOutCubic(static_cast<uint8_t>(ratio));

        driver.MAX7219_SetBrightness(brightness);

        if (ratio >= 1.0f)
        {
            delay(500);
            if (lowBattery)
            {
                driver.DisplayText("ttab ol ", 0);
                delay(5000);
            }
            return false;
        }
        delay(10);
        return true;
    }

    void drawMenu(const TimeControl &timeControl)
    {
        displayLeftSideTime(timeControl.startTime);
        
        static char rightChars[5]; // Buffer for 4 characters + null terminator
        sprintf(rightChars, "+ %02d", timeControl.increment);
        contentStaging.setSideContent(rightChars, false, false);
    }

    void drawGame(uint32_t leftTimeMs, uint32_t rightTimeMs, bool leftSideTurn)
    {
        uint32_t leftTimeSeconds = leftTimeMs / 1000;
        uint32_t rightTimeSeconds = rightTimeMs / 1000;
        uint32_t leftTimeHalfSeconds = leftTimeMs / 500; // blink colon every 500 ms
        uint32_t rightTimeHalfSeconds = rightTimeMs / 500; // blink colon every 500 ms
        displayLeftSideTime(leftTimeSeconds, leftTimeHalfSeconds % 2 == 0 || !leftSideTurn);
        displayRightSideTime(rightTimeSeconds, rightTimeHalfSeconds % 2 == 0 || leftSideTurn);
    }

    void drawPause(uint32_t leftTimeMs, uint32_t rightTimeMs)
    {
        static bool blink = true;
        EVERY_N_MILLIS(500) { blink = !blink; }
        if (blink)
        {
            uint32_t leftTimeSeconds = leftTimeMs / 1000;
            uint32_t rightTimeSeconds = rightTimeMs / 1000;
            displayLeftSideTime(leftTimeSeconds);
            displayRightSideTime(rightTimeSeconds);
        }
        else
            contentStaging = Content();
    }

    void drawPlayerFlagged(uint32_t leftTimeMs, uint32_t rightTimeMs)
    {
        static bool blink = false;
        EVERY_N_MILLIS(250) { blink = !blink; }
        
        uint32_t leftTimeSeconds = leftTimeMs / 1000;
        uint32_t rightTimeSeconds = rightTimeMs / 1000;

        if (blink || leftTimeMs != 0)
            displayLeftSideTime(leftTimeSeconds);
        else
            contentStaging.setSideContent("    ", true, false);

        if (blink || rightTimeMs != 0)
            displayRightSideTime(rightTimeSeconds);
        else
            contentStaging.setSideContent("    ", false, false);
    }

    /// @brief Updates the display with the current character buffer.
    /// If the content has not changed, it does nothing.
    /// @param forceUpdate If true, the display is updated regardless of whether the content has changed.
    void updateDisplay(bool forceUpdate = false)
    {
        if (flashActive)
        {
            if ((millis() - flashStartMs) >= flashDurationMs)
            {
                flashActive = false;
                driver.MAX7219_SetBrightness(defaultBrightness);
            }
        }

        if (!forceUpdate && contentStaging == contentActive)
            return;
        contentActive = contentStaging;
        driver.DisplayContent(contentActive);
    }

private:

    MAX7219 driver;
    Content contentStaging;
    Content contentActive;

    static const char normalModeBrightness = 0x0C;
    static const char lowBatteryBrightness = 0x02;
    bool lowBattery = false;
    char defaultBrightness = normalModeBrightness;

    static const uint32_t flashDurationMs = 60; // Duration for flashing content
    uint32_t flashStartMs = UINT32_MAX;
    bool flashActive = false;

    void displaySideTime(uint32_t totalSeconds, bool left, bool colon = true)
    {
        static char buffer[5]; // Buffer for 4 digits + null terminator
        uint32_t minutes = timeControlMinutes(totalSeconds);
        uint32_t seconds = timeControlSecondsRemainder(totalSeconds);
        snprintf(buffer, 5, "%2d", minutes);
        snprintf(buffer + 2, 3, "%02d", seconds);
        contentStaging.setSideContent(buffer, left, colon);
    }

    void displayLeftSideTime(uint32_t totalSeconds, bool colon = true)
    {
        displaySideTime(totalSeconds, true, colon);
    }

    void displayRightSideTime(uint32_t totalSeconds, bool colon = true)
    {
        displaySideTime(totalSeconds, false, colon);
    }
};