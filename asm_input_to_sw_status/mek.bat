@echo off
rmdir /q /s build_rls
mkdir build_rls

"C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/main.lst" -e"build_rls/main.err" -o"build_rls/main.o" "src/main.asm" 
"C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mplink.exe" -p12lf1571 -w -m"build_rls/SW0037-02v01pre.map" -z__MPLAB_BUILD_RLS=1  -o"build_rls/SW0037-02v01pre.cof" build_rls/main.o
type build_rls\main.err

rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/12LF1571.lst" -e"build_rls/12LF1571.err" -o"build_rls/12LF1571.o" "src/12LF1571.asm" 
rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/Crc16.lst" -e"build_rls/Crc16.err" -o"build_rls/Crc16.o" "src/Crc16.asm" 
rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/RamTest.lst" -e"build_rls/RamTest.err" -o"build_rls/RamTest.o" "src/RamTest.asm" 
rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/VersionOut.lst" -e"build_rls/VersionOut.err" -o"build_rls/VersionOut.o" "src/VersionOut.asm" 
rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/EnterSleep.lst" -e"build_rls/EnterSleep.err" -o"build_rls/EnterSleep.o" "src/EnterSleep.asm" 
rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mpasmx.exe" -w0 -q -p12lf1571 -l"build_rls/error.lst" -e"build_rls/error.err" -o"build_rls/error.o" "src/error.asm" 
rem "C:\Program Files (x86)\Microchip\MPLABX\mpasmx\mplink.exe"    -p12lf1571  -w  -m"build_rls/production.map"   -z__MPLAB_BUILD_RLS=1  -o"build_rls/Panther1.X.production.cof"  build_rls/12LF1571.o build_rls/Crc16.o build_rls/RamTest.o build_rls/VersionOut.o build_rls/EnterSleep.o build_rls/error.o
rem "checksum\P12lf1571V01Pre1-HexCrc.exe" -d build_rls\Panther1.X.production.hex build_rls\pa.bin build_rls\SW0014-30v05pre.hex build_rls\padebug.hex
