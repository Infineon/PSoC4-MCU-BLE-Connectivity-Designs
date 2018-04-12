:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: File Name: mk.bat
::
:: Version: 1.0
::
:: Description:
::  This batch file is generating list of symbols that are to be exported from
::  Bootloader project to the Bootloadable.
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Copyright 2014-2015, Cypress Semiconductor Corporation. All rights reserved.
:: This software is owned by Cypress Semiconductor Corporation and is protected
:: by and subject to worldwide patent and copyright laws and treaties.
:: Therefore, you may use this software only as provided in the license agreement
:: accompanying the software package from which you obtained this software.
:: CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
:: WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
:: IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@echo off
setlocal enableextensions enabledelayedexpansion
::
::  This section contains Bootloader's project name and path to sources,
::  Bootloadable's project name, path to LinkerScripts\ folder
::  These settings are required in order for script to works as expected
::
set LOADER_PRJ_NAME=BLE_OTA_FixedStack_Bootloader
set LOADER_PRJ_PATH=%~dp0..\
set LOADABLE_PRJ_NAME=BLE_OTA_FixedStack_Bootloadable
set OUTPUT_DIR=%~dp0..\..\%LOADABLE_PRJ_NAME%.cydsn\LinkerScripts\

::
::  PSoC Creator location options
::
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO 32BIT)
:32BIT
set PROGRAM_FILES_PATH=c:\Program Files
GOTO DONE_PC_PATH
:64BIT
set PROGRAM_FILES_PATH=c:\Program Files (x86)
GOTO DONE_PC_PATH
:DONE_PC_PATH

set CREATOR_VERSION=4.1
set CREATOR_LOCATION=%PROGRAM_FILES_PATH%\Cypress\PSoC Creator

::
::  Compiler choice. Currently only GCC 5.4.1, MDK and IAR compilers are supported.
::
set GCC_COMPILER=ARM_GCC_541
set IAR_COMPILER=ARM_IAR_Generic
set MDK_COMPILER=ARM_MDK_Generic

set COMPILER=%GCC_COMPILER%

::
:: Compilation mode. By default it is debug.
:: If compilation mode was changed in PSoC Creator - this section
:: should be updated as well.
::
set COMPILE_OPTION_DEBUG=Debug
set COMPILE_OPTION_RELEASE=Release
set COMPILE_OPTION=%COMPILE_OPTION_DEBUG%

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Code below this point should not be modified in most cases.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Bootloader's output folder.
set BUILD_OUTPUT_DIR=CortexM0\%COMPILER%\%COMPILE_OPTION%\

:: Bootloader's *.elf file path.
set LOADER_ELF_FILE=%LOADER_PRJ_PATH%%BUILD_OUTPUT_DIR%%LOADER_PRJ_NAME%.elf

:: This option allows to enable removing all temporary files. By default,
:: temporary files are saved. To change this - replace 'YES' with 'NO'.
set KEEP_TMP_FILES=YES

:: Path to utility arm-none-eabi-nm.exe that parses ELF file
set UTILS_NM=%CREATOR_LOCATION%\%CREATOR_VERSION%\PSoC Creator\import\gnu\arm\5.4.1\bin\arm-none-eabi-nm.exe

::
:: Uncomment this section to verify if all of that paths and option set properly
::
::echo --------------------------------------------
::echo LOADER_PRJ_NAME=%LOADER_PRJ_NAME%
::echo.
::echo LOADABLE_PRJ_NAME=%LOADABLE_PRJ_NAME%
::echo.
::echo COMPILER=%COMPILER%
::echo.
::echo COMPILE_OPTION=%COMPILE_OPTION_DEBUG%
::echo.
::echo LOADER_PRJ_PATH=%LOADER_PRJ_PATH%
::echo.
::echo LOADABLE_PRJ_NAME=%LOADABLE_PRJ_NAME%
::echo.
::echo BUILD_OUTPUT_DIR=%BUILD_OUTPUT_DIR%
::echo.
::echo OUTPUT_DIR=%OUTPUT_DIR%
::echo.
::echo LOADER_ELF_FILE=%LOADER_ELF_FILE%
::echo.
::echo UTILS_NM=%UTILS_NM%
::echo.
::echo KEEP_TMP_FILES=%KEEP_TMP_FILES%
::echo --------------------------------------------

:: Variable's list
:: These files should be present at Scripts\ folder
set SECTION_LIST=section_list.txt
set SEARCHING_SYMBOLS=symbol_list.txt

:: These temporary files would be generated
set PARSED_ELF=ParsedElf.txt
set EXTRACTED_BLE_INFO=Extracted_BLE_Info.txt
set BLE_INFO_ALL=Ble_Info_All.txt
set BLE_INFO_ALL_SORTED=Ble_Info_All_Sorted.txt

:: Generated output file with the list of symbols and their addresses for GCC, MDK and IAR compiler
set LOADABLE_PROVIDE_GCC=%OUTPUT_DIR%BootloaderSymbolsGcc.ld
set LOADABLE_SYMDEF_MDK=%OUTPUT_DIR%BootloaderSymbolsMdk.txt
set LOADABLE_DEFINITIONS_IAR=%OUTPUT_DIR%BootloaderSymbolsIar.icf
set LOADABLE_HEADER=%OUTPUT_DIR%BootloaderSymbols.h


set ERROR_IS_PRESENT="0"

:: Check if specified Bootloader project folder exists
if NOT exist "%LOADER_PRJ_PATH%" (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find Bootloader's project folder %LOADER_PRJ_PATH%
    echo Please check if it specified correctly.
    echo ------------------------------------------------------------------------------
    set ERROR_IS_PRESENT="1"
    goto endPoint
)

:: Check if specified Bootloadable project folder exists
if NOT exist "%OUTPUT_DIR%" (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find Bootloadable's project \LinkerScripts folder %OUTPUT_DIR%
    echo Please check if it specified correctly.
    echo ------------------------------------------------------------------------------
    set ERROR_IS_PRESENT="1"
    goto endPoint
)

if NOT exist "%UTILS_NM%" (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find utilities folder "%UTILS_NM%"
    echo Please check if path to PSoC Creator specified correctly.
    echo ------------------------------------------------------------------------------
    set ERROR_IS_PRESENT="1"
    goto endPoint
)

:: Check if "%LOADER_ELF_FILE%" file exists
if NOT exist "%LOADER_ELF_FILE%" (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find %LOADER_ELF_FILE% file, perhaps, it was not generated.
    echo ------------------------------------------------------------------------------
    set ERROR_IS_PRESENT="1"
    goto endPoint
)

:: Parsing ELF file: <address> <section> <symbol name>
echo Parsing ELF file...
"%UTILS_NM%" "%LOADER_ELF_FILE%" > %PARSED_ELF%

:: Check if "symbol_list.txt" file exist
if NOT exist %SEARCHING_SYMBOLS% (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find %SEARCHING_SYMBOLS%, it should be located in the Scripts\ folder.
    echo ------------------------------------------------------------------------------
    set ERROR_IS_PRESENT="1"
    goto endPoint
)

:: Check if %SEARCHING_SYMBOLS% file is not empty
call :CheckEmpty %SEARCHING_SYMBOLS%

:: Extract BLE-related info, %SEARCHING_SYMBOLS% contains prefixes for searching.
:: Currently it is just "CyBle_". BLE component name should be set as "CyBle"
:: to be able to pick up all the APIs.
echo Filtering exported symbols ...
@echo off > %EXTRACTED_BLE_INFO%
findstr /i /g:%SEARCHING_SYMBOLS% %PARSED_ELF% > %EXTRACTED_BLE_INFO%

:: Check if %EXTRACTED_BLE_INFO% file was generated
if NOT exist %EXTRACTED_BLE_INFO% (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find %EXTRACTED_BLE_INFO%, it should be generated from Bootloader's ELF file.
    echo Possible reason: there were no symbols from %SEARCHING_SYMBOLS% file in Bootloader's ELF file
    echo ------------------------------------------------------------------------------
    goto endPoint
)

:: Check if %EXTRACTED_BLE_INFO% file is not empty
call :CheckEmpty %EXTRACTED_BLE_INFO%

:: Check if %SECTION_LIST% file exist
if NOT exist %SECTION_LIST% (
    echo ------------------------------------------------------------------------------
    echo ERROR: Can't find %SECTION_LIST%, it should be located in the Scripts\ folder.
    echo ------------------------------------------------------------------------------
    goto endPoint
)

:: Check if %SECTION_LIST% file is not empty
call :CheckEmpty %SECTION_LIST%

:: Extract all the symbols for listed sections(for example: T,t,B,b,D,d,R,r,a)
:: T/t - Global/local text section, D/d -Global/local data section,
:: R/r - Global/local read-only section, B/b - Global/local bss section,
:: a - absolute symbols section.
:: The searching procedure is NOT case-sensitive (related to section symbols only).
:: To have it case sensitive remove "/i" key.
echo Filtering by section list ...
findstr /i /g:%SECTION_LIST% %EXTRACTED_BLE_INFO% > %BLE_INFO_ALL%

:: Sorting by address
echo Sorting by address ...
@echo off
sort %BLE_INFO_ALL% > %BLE_INFO_ALL_SORTED%

:: Open %LOADABLE_PROVIDE_GCC% file for generating the list of PROVIDEs
if [%COMPILER%] == [%MDK_COMPILER%] (
    @echo off > "%LOADABLE_SYMDEF_MDK%"
    echo ^#^<SYMDEFS^>^# >> "%LOADABLE_SYMDEF_MDK%"
    @echo off > "%LOADABLE_HEADER%"
) else if [%COMPILER%] == [%GCC_COMPILER%] (
    @echo off > "%LOADABLE_PROVIDE_GCC%"
) else if [%COMPILER%] == [%IAR_COMPILER%] (
    @echo off > "%LOADABLE_DEFINITIONS_IAR%"
)

:: Main cycle for output symbols generation
echo Generating output file(s) ...
SETLOCAL ENABLEDELAYEDEXPANSION
for /f "tokens=1,2,3* delims= " %%a in (%BLE_INFO_ALL_SORTED%) do (

    set EXPORT_NAME=%%c
    set MDK_SYMBOL_TYPE=%%b

    :: Special GCC variables
    if [%%c] == [__cy_regions]               set EXPORT_NAME=Bootloader%%c
    if [%%c] == [__cy_regions_end]           set EXPORT_NAME=Bootloader%%c
    if [%%c] == [__cy_region_num]            set EXPORT_NAME=Bootloader%%c
    if [%%c] == [__libc_init_array]          set EXPORT_NAME=Bootloader%%c
    :: special MDK variables
    if [%%c] == [__main]                     set EXPORT_NAME=Bootloader%%c
    if [%%c] == [Image$$DATA$$ZI$$Limit]     set EXPORT_NAME=Bootloader__%%c
    :: special IAR variables
    if [%%c] == [__iar_data_init3]           set EXPORT_NAME=Bootloader%%c
    if [%%c] == [__iar_program_start]        set EXPORT_NAME=Bootloader%%c


    :: MDK symbol define type
    if [%%b] == [B] set MDK_SYMBOL_TYPE=D
    if [%%b] == [b] set MDK_SYMBOL_TYPE=D
    if [%%b] == [D] set MDK_SYMBOL_TYPE=D
    if [%%b] == [d] set MDK_SYMBOL_TYPE=D
    if [%%b] == [T] set MDK_SYMBOL_TYPE=T
    if [%%b] == [t] set MDK_SYMBOL_TYPE=T
    if [%%b] == [R] set MDK_SYMBOL_TYPE=T
    if [%%b] == [r] set MDK_SYMBOL_TYPE=T
    if [%%b] == [a] set MDK_SYMBOL_TYPE=T
    @rem Fix to CDT 250035. Variables shall not have "| 0x01" is their address.
    if ["!EXPORT_NAME:~0,6!"] == ["cyBle_"] set MDK_SYMBOL_TYPE=D

    if [%COMPILER%] == [%IAR_COMPILER%] (
    set quote=^?
    if ["!EXPORT_NAME:~0,1!"] == ["!quote!"] (
        set EXPORT_NAME=!EXPORT_NAME:~2!
        )
    )

    if [%%c] == [__bss_end__] (
        if [%COMPILER%] == [%GCC_COMPILER%] (
            echo BOOTLOADER_RAM_SIZE = (0x%%a - 0x20000000^); >> "%LOADABLE_PROVIDE_GCC%"
        )
    ) else (
        if [%COMPILER%] == [%GCC_COMPILER%] (
            echo PROVIDE(!EXPORT_NAME! = 0x%%a^); >> "%LOADABLE_PROVIDE_GCC%"
        ) else  if [%COMPILER%] == [%MDK_COMPILER%] (
            echo 0x%%a !MDK_SYMBOL_TYPE! !EXPORT_NAME! >> "%LOADABLE_SYMDEF_MDK%"

            echo #define !EXPORT_NAME! 0x%%a >> "%LOADABLE_HEADER%"
        ) else if [%COMPILER%] == [%IAR_COMPILER%] (
            @rem Skip the names with :: inside them. These names are generated
            @rem by IAR 7.70 and above for static variables defined inside
            @rem the functions.
            if /i "!EXPORT_NAME:::=!" == "!EXPORT_NAME!" (
                if [!MDK_SYMBOL_TYPE!] == [T] (
                    echo define exported symbol !EXPORT_NAME! = 0x%%a^|0x01; >> "%LOADABLE_DEFINITIONS_IAR%"
                )else (
                    echo define exported symbol !EXPORT_NAME! = 0x%%a; >> "%LOADABLE_DEFINITIONS_IAR%"
                )
            )
        )
    )
)

goto endPoint

:: Subroutine to check if file is empty
:CheckEmpty
if %~z1 == 0 (
    echo ERROR: %1 file is empty
    goto endPoint
) else (
    exit /b
)

:endPoint
:: if set "NO" then delete tmp files
if "%KEEP_TMP_FILES%" == "NO" (
    if exist "%PARSED_ELF%" del "%PARSED_ELF%"
    if exist %EXTRACTED_BLE_INFO% del %EXTRACTED_BLE_INFO%
    if exist %BLE_INFO_ALL% del %BLE_INFO_ALL%
    if exist %BLE_INFO_ALL_SORTED% del %BLE_INFO_ALL_SORTED%
)

if %ERROR_IS_PRESENT% == "1" (
    echo ------------------------------------------------------------------------------
    echo Please refer to the example project datasheet for details on configuring
    echo this script.
    echo ------------------------------------------------------------------------------
)
pause
