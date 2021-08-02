REM Build script for Windows.
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files: " %cFilenames%

SET assembly=server
SET compilerFlags=-g -Wvarargs -Wall
SET output=-o bin/%assembly%.exe
SET linkerFlags=-luser32
SET includeDirectory=-Isrc

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% %includeDirectory% %output% %linkerFlags%


