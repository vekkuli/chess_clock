#define MAX7219_CLK_PIN 15
#define MAX7219_DIN_PIN 16
#define MAX7219_CS_PIN 18

#define DOWN_BUTTON_PIN 6
#define UP_BUTTON_PIN 7
#define STOP_BUTTON_PIN 8
#define PLAY_BUTTON_PIN 9

#define TURN_SWITCH_PIN 4

#define LBO_PIN 2

#include "EasyButton.h"
#include "serial_debug.h"
#include "system.h"
#include "display.h"
#include "game.h"
#include "timecontrol.h"

Display System::display = Display(MAX7219_CLK_PIN, MAX7219_DIN_PIN, MAX7219_CS_PIN);
Game System::game;

FSM_INITIAL_STATE(System, Startup)

//
// Buttons (all 35 ms debounce time, pulled up, active low)
EasyButton playButton(PLAY_BUTTON_PIN);
EasyButton stopButton(STOP_BUTTON_PIN);
EasyButton upButton(UP_BUTTON_PIN);
EasyButton downButton(DOWN_BUTTON_PIN);
EasyButton turnSwitch(TURN_SWITCH_PIN, 35U, true, false);

void onPlayButtonPressed() { System::dispatch(PlayButtonPressed()); }
void onStopButtonPressed() { System::dispatch(StopButtonPressed()); }
void onStopButtonHeld() { System::dispatch(StopButtonHeld()); }
void onUpButtonPressed() { System::dispatch(UpButtonPressed()); }
void onDownButtonPressed() { System::dispatch(DownButtonPressed()); }

void setup()
{
    System::start(); // Start the state machine
    System::display.begin();

    pinMode(LBO_PIN, INPUT);

    playButton.begin();
    playButton.onPressed(onPlayButtonPressed);

    stopButton.begin();
    stopButton.onPressed(onStopButtonPressed);
    stopButton.onPressedFor(1000, onStopButtonHeld);

    upButton.begin();
    upButton.onPressed(onUpButtonPressed);

    downButton.begin();
    downButton.onPressed(onDownButtonPressed);

    turnSwitch.begin();
}

void loop()
{
    System::display.setLowBatteryState(digitalRead(LBO_PIN) == LOW);
    playButton.read();
    stopButton.read();
    upButton.read();
    downButton.read();
    if (System::game.setTurnSwitchState(turnSwitch.read()))
        System::display.flash();
    System::current_state_ptr->tick();
    System::display.updateDisplay();
}