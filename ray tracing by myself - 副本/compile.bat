@echo off
cd "C:\Users\johna\Desktop\ray tracing by myself"
"C:\Program Files (x86)\Dev-Cpp\MinGW32\bin\g++.exe" -std=c++11 main.cpp -o "ray tracing by myself.exe"
if %errorlevel% equ 0 (
    echo 编译成功
    "ray tracing by myself.exe"
    echo 程序已运行
) else (
    echo 编译失败
    pause
)