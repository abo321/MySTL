@echo off

echo start to compile all examples
echo.

cl /wd 4530 /wd 4996 /nologo my_rb_tree_test.cpp my_rb_tree.cpp
echo.

del *.obj

echo done. bye.
pause
