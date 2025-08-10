#pragma once

struct TimeControl
{
    uint32_t startTime; // Seconds
    uint32_t increment; // Seconds
    const char* name;
};


inline uint32_t timeControlMinutes(uint32_t totalSeconds)
{
    return totalSeconds / 60;
}

inline uint32_t timeControlSecondsRemainder(uint32_t totalSeconds)
{
    return totalSeconds % 60;
}