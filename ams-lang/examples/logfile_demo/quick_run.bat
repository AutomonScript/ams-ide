@echo off
setlocal EnableDelayedExpansion

:: Enable ANSI color codes for Windows 10+
for /F %%a in ('echo prompt $E ^| cmd') do set "ESC=%%a"

echo %ESC%[36m=== Production Log Monitoring Demo - Quick Run ===%ESC%[0m
echo.

:: Get script directory and project root (two levels up)
pushd "%~dp0..\.."

echo %ESC%[90m[INFO] Working directory: %CD%%ESC%[0m
echo.

:: Compile
echo %ESC%[33m[1/2] Compiling...%ESC%[0m
".\build\ams.exe" build "examples\logfile_demo.ams"

if %ERRORLEVEL% NEQ 0 (
    echo %ESC%[31m[ERROR] Compilation failed!%ESC%[0m
    popd
    exit /b 1
)

echo %ESC%[32m[SUCCESS] Compiled!%ESC%[0m
echo.

:: Run
echo %ESC%[33m[2/2] Running demo (will run for ~15 seconds)...%ESC%[0m
echo %ESC%[90mPress Ctrl+C to stop early%ESC%[0m
echo.

".\logfile_demo.exe"

echo.
echo %ESC%[32m[DONE] Demo complete!%ESC%[0m

:: Return to original directory
popd
endlocal
