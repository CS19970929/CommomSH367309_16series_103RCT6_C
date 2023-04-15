
@echo.
echo ÇåÀíÖÐ¡­¡­

copy Objects\output.hex ..\output(mdk).hex
copy Objects\output.bin ..\output(mdk).bin


:: Project/
::del>nul 2>nul *.uvoptx   /s /q 
del>nul 2>nul *.uvguix.* /s /q  
del>nul 2>nul *.scvd     /s /q 

:: Project/DebugConfig
del>nul 2>nul *.dbgconf /s /q  

:: Project/Listings
del>nul 2>nul *.lst /s /q  

:: Project/Objects
del>nul 2>nul *.htm /s /q 
del>nul 2>nul *.crf /s /q 
del>nul 2>nul *.dep /s /q 
del>nul 2>nul *.lnp /s /q 
del>nul 2>nul *.sct /s /q 
del>nul 2>nul *.d   /s /q 
del>nul 2>nul *.o   /s /q 


@echo.
Echo ok!!!!
@echo. 

exit
