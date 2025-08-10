#pragma once

#include "serial_debug.h"
#include "display.h"
#include "game.h"

#define TINYFSM_NOSTDLIB
#include "tinyfsm.hpp"

// Fwd declare all state
struct Startup;
struct Menu;
struct GameInitialized;
struct GameRunning;
struct GamePaused;
struct PlayerFlagged;

//
// Events

struct PlayButtonPressed : tinyfsm::Event { };
struct StopButtonPressed : tinyfsm::Event { };
struct StopButtonHeld : tinyfsm::Event { };
struct UpButtonPressed : tinyfsm::Event { };
struct DownButtonPressed : tinyfsm::Event { };


/// @brief System State Machine
struct System : tinyfsm::Fsm<System>
{
    virtual void react(PlayButtonPressed const &) { }
    virtual void react(StopButtonPressed const &) { }
    virtual void react(StopButtonHeld const &) { }
    virtual void react(UpButtonPressed const &) { }
    virtual void react(DownButtonPressed const &) { }

    virtual void entry(void) { }
    virtual void exit(void) { }

    virtual void tick() = 0;

    static Display display;
    static Game game;
};

//
// States

struct Startup : System
{
    void entry() override
    {
        DEBUG_PRINTLN("State: Startup");
    }

    void tick() override
    {
        if (!display.drawStartupAnimation())
            transit<Menu>();
    }
};

struct Menu : System
{
    void entry() override
    {
        DEBUG_PRINTLN("State: Menu");
    }

    void react(PlayButtonPressed const &) override
    {
        game.initializeGame();
        transit<GamePaused>();
    }

    void react(UpButtonPressed const &) override
    {
        game.nextTimeControl();
    }

    void react(DownButtonPressed const &) override
    {
        game.prevTimeControl();
    }

    void tick() override
    {
        display.drawMenu(game.timeControl());
    }
};

struct GameRunning : System
{
    void entry() override
    {
        DEBUG_PRINTLN("State: GameRunning");
        game.continueGame();
    }

    void react(PlayButtonPressed const &) override
    {
        transit<GamePaused>();
    }

    void react(StopButtonPressed const &) override
    {
        transit<GamePaused>();
    }

    void react(StopButtonHeld const &) override
    {
        transit<Menu>();
    }

    void tick() override
    {
        game.tick();
        display.drawGame(game.leftTimeMs(), game.rightTimeMs(), game.isLeftSideTurn());

        if (game.playerFlagged())
        {
            transit<PlayerFlagged>();
        }
    }
};

struct GamePaused : System
{
    void entry() override
    {
        DEBUG_PRINTLN("State: GamePaused");
        game.pauseGame();
    }

    void react(PlayButtonPressed const &) override
    {
        transit<GameRunning>();
    }

    void react(StopButtonHeld const &) override
    {
        transit<Menu>();
    }

    void tick() override
    {
        display.drawPause(game.leftTimeMs(), game.rightTimeMs());
    }
};

struct PlayerFlagged : System
{
    void entry() override
    {
        DEBUG_PRINTLN("State: PlayerFlagged");
    }

    void react(StopButtonHeld const &) override
    {
        transit<Menu>();
    }

    void tick() override
    {
        display.drawPlayerFlagged(game.leftTimeMs(), game.rightTimeMs());
    }
};
