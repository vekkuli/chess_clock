@rem ------------------------------------------------------------------------------------------------------------------------
@rem Arduino build from command line.
@rem 24/07/2020 Modified for SparkFun Pro Micro
@rem
@rem This uses arduino_debug.exe to compile the sketch, then avrdude to load it to the Pro Micro.  When prompted, press
@rem the Pro Micro's reset button (that you connected).
@rem
@rem Put this file in the Arduino project folder (with MyProg.ino)
@rem 
@rem The board type is correct for the Pro-Micro but you'll need to edit the port, Arduino.exe location, Avrdude location.
@rem
@rem The port is the bootload port which appears in Device Manager only briefly after you reset the Pro-Micro. Connecting 
@rem reset twice quickly makes that port stay around for a few seconds longer.  Even with that, sometimes Windows is not 
@rem quick enough to show it.
@rem
@rem Arduino build: https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
@Rem
@rem Pro micro: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide
@rem
@rem Tested on Windows-XP and Windows-10.
@rem ------------------------------------------------------------------------------------------------------------------------

echo off
setlocal EnableDelayedExpansion
set "startTime=%time: =0%"

@rem ------ edited-settings... --------------------------------------------------------------------

@rem Arduino Board Type
@REM ==================
@rem SparkFun Pro Micro
set brd=SparkFun:avr:promicro:cpu=16MHzatmega32U4

@rem Port name
@rem =========
set prt=COM3

@rem Hex file to upload
@rem ==================
set hex=build\chess_clock_fw.ino.with_bootloader.hex

@rem Avrdude location
@rem ================
@rem Windows-10
set avrdude=%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17
@rem ------- ... edited-settings ------------------------------------------------------------------

if %errorlevel% == 0 goto deliver
  echo "FAIL."
pause
exit /b

@rem ----------------------------------------------------------------------------------------------
:deliver
@rem get the hex file from the temporary folder
echo .
echo RESET THE PRO MICRO.
echo .
@rem use count to prevent infinite loop
set /a count=0

@rem ----------------------------------------------------------------------------------------------
@rem wait for the port to appear.
:loop
  mode > ports.lis
@rem mode side-effect makes Ard lamp flash! (what about other serial devices?)
findstr %prt% ports.lis
if %errorlevel% == 0 goto do_load
@rem 0 = not found, 1 = found

set /a count=%count%+1
echo Looking for %prt%  %count%

if %count% leq 500 goto loop


echo Failed to find %prt%
pause
goto done_or_not

:do_load

@rem ----------------------------------------------------------------------------------------------
@rem Load the hex file into the Pro Micro.
%avrdude%\bin\avrdude.exe -C%avrdude%\etc\avrdude.conf -v -V -patmega32u4 -cavr109 -P%prt% -b57600 -D -Uflash:w:%hex%:i

goto postload
:postload       

set "endTime=%time: =0%"
@rem Get elapsed time:
set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!startTime:%time:~8,1%=%%100)*100+1!"
set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100)"
@rem Convert elapsed time to HH:MM:SS:CC format:
set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"
echo Elapsed:  %hh:~1%%time:~2,1%%mm:~1%%time:~2,1%%ss:~1%%time:~8,1%%cc:~1%
@rem
pause done_or_not
exit /b
@rem ------------------------------------------------------------------------------------------------------------------------