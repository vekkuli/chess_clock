#pragma once

#include "timecontrol.h"


const TimeControl allTimeControls[] = {
    { 1  * 60,      0,      "Bullet"    },
    { 1  * 60,      1,      "Bullet"    },
    { 2  * 60,      1,      "Bullet"    },
    { 3  * 60,      0,      "Blitz"     },
    { 3  * 60,      2,      "Blitz"     },
    { 5  * 60,      0,      "Blitz"     },
    { 10 * 60,      0,      "Rapid"     },
    { 15 * 60,      10,     "Rapid"     },
    { 30 * 60,      0,      "Rapid"     }
};

constexpr uint8_t numTimeControls = sizeof(allTimeControls) / sizeof(TimeControl);


struct Timer
{
    void init(uint32_t startTime)
    {
        timeRemainingMs = startTime;
        latchReferenceTime();
    }

    void latchReferenceTime()
    {
        referenceTimeMs = timeRemainingMs;
        referenceTimePointMs = millis();
    }

    void addIncrement(uint32_t incrementMs)
    {
        timeRemainingMs += incrementMs;
    }

    uint32_t updateRemainingTime()
    {
        uint32_t delta = millis() - referenceTimePointMs;
        if (delta >= referenceTimeMs)
            timeRemainingMs = 0;
        else
            timeRemainingMs = referenceTimeMs - delta;
        return timeRemainingMs;
    }

    uint32_t referenceTimeMs = 0;
    uint32_t referenceTimePointMs = 0;
    uint32_t timeRemainingMs = 0;
};


class Game
{
public:

    void nextTimeControl()
    {
        _currentTimeCtlIdx = min(_currentTimeCtlIdx + 1, numTimeControls - 1);
    }

    void prevTimeControl()
    {
        _currentTimeCtlIdx = max(_currentTimeCtlIdx - 1, 0);
    }

    const TimeControl& timeControl() const
    {
        return allTimeControls[_currentTimeCtlIdx];
    }

    uint32_t leftTimeMs() const
    {
        return _leftTimer.timeRemainingMs;
    }

    uint32_t rightTimeMs() const
    {
        return _rightTimer.timeRemainingMs;
    }

    bool isLeftSideTurn() const
    {
        return _turnSwitchState;
    }

    void initializeGame()
    {
        _playerFlagged = false;
        _gameRunning = false;
        _leftTimer.init(timeControl().startTime * 1000);
        _rightTimer.init(timeControl().startTime * 1000);
    }

    void pauseGame()
    {
        _gameRunning = false;
    }

    void continueGame()
    {
        _gameRunning = true;
        _leftTimer.latchReferenceTime();
        _rightTimer.latchReferenceTime();
    }

    bool playerFlagged() const
    {
        return _playerFlagged;
    }

    bool setTurnSwitchState(bool newState)
    {
        if (_turnSwitchState == newState)
            return false; // No change in state

        _turnSwitchState = newState;
        if (!_gameRunning || _playerFlagged)
            return false; // Do not increment timers if the game is not running

        if (isLeftSideTurn())
        {
            _rightTimer.addIncrement(timeControl().increment * 1000);
            _leftTimer.latchReferenceTime();
        }
        else
        {
            _leftTimer.addIncrement(timeControl().increment * 1000);
            _rightTimer.latchReferenceTime();
        }
        return true;
    }

    void tick()
    {
        if (playerFlagged())
            return;

        if (isLeftSideTurn())
        {
            _playerFlagged = _leftTimer.updateRemainingTime() == 0;
        }
        else
        {
            _playerFlagged = _rightTimer.updateRemainingTime() == 0;
        }
    }

private:
    uint8_t _currentTimeCtlIdx = 4;

    bool _gameRunning = false;

    bool _turnSwitchState = false;

    Timer _leftTimer;
    Timer _rightTimer;

    bool _playerFlagged = false;
};