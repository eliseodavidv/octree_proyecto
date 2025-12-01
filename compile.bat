@echo off
echo Compilando proyecto Octree...
g++ -std=c++17 -O2 main.cpp -o octree_demo.exe
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilacion exitosa!
    echo Ejecutable: octree_demo.exe
) else (
    echo.
    echo Error en la compilacion
)
pause
