@echo off

set New=CommomSH367309_16series_103RCT6_C
echo Your New Project Name is %New%.

ren *.uvoptx %New%.uvoptx
ren *.uvprojx %New%.uvprojx

del /f /q *.uvguix.*

cd Objects
del /f /s /q *.*

cd ../Listings
del /f /s /q *.*

cd ../DebugConfig
del /f /s /q *.*

set New=
