@echo off

echo start to compile all examples
echo.

cl /wd 4530 /wd 4996 /nologo my_list_test.cpp
echo.

del *.obj

echo done. bye.
pause
