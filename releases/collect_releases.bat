@echo off
:: git commit hash
FOR /F "tokens=*" %%g IN ('git rev-parse HEAD') do (SET VAR=%%g)
::local time
for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set ldt=%%j
set ldt=%ldt:~0,4%-%ldt:~4,2%-%ldt:~6,2% %ldt:~8,2%:%ldt:~10,2%:%ldt:~12,6%
set date=%ldt%
set date=%date::=-%
::echo Date:    %ldt% - %date%
::echo Version: %VAR%
set dirname=%Date% %VAR%
echo Dir: %dirname%
mkdir "%dirname%"

SET plugname=Transcriber
SET win32_label=_32
SET x64_label=_64
SET VST3_label=_VST3
SET VST2_label=_VST2
SET AAX_label=_AAX
SET PC_label=_WIN
SET VST3_ext=.vst3
SET VST2_ext=.dll
SET AAX_ext=.aaxplugin
::VST3
COPY "..\build-win\vst3\Win32\bin\%plugname%%win32_label%%VST3_ext%" "%dirname%\%plugname%%PC_label%%win32_label%%VST3_label%%VST3_ext%"
COPY "..\build-win\vst3\x64\bin\%plugname%%win64_label%%VST3_ext%" "%dirname%\%plugname%%PC_label%%x64_label%%VST3_label%%VST3_ext%"
::VST2
COPY "..\build-win\vst2\Win32\bin\%plugname%%win32_label%%VST2_ext%" "%dirname%\%plugname%%PC_label%%win32_label%%VST2_label%%VST2_ext%"
COPY "..\build-win\vst2\x64\bin\%plugname%%win64_label%%VST2_ext%" "%dirname%\%plugname%%PC_label%%x64_label%%VST2_label%%VST2_ext%"
::AAX
COPY "..\build-win\aax\bin\Transcriber.aaxplugin\Contents\x64\%plugname%%AAX_ext%" "%dirname%\%plugname%%PC_label%%x64_label%%AAX_label%%AAX_ext%"
COPY "..\build-win\aax\bin\Transcriber.aaxplugin\Contents\Win32\%plugname%%AAX_ext%" "%dirname%\%plugname%%PC_label%%win32_label%%AAX_label%%AAX_ext%"