call xcopy /y /c /h /r "E:\CubeMx\H750_sd\APP\MDK-ARM\h7_usb\h7_usb.bin" "F:\SD_BIN"
call del "F:\SD_BIN\demo_old.bin"
call ren "F:\SD_BIN\demo.bin" demo_old.bin
call ren "F:\SD_BIN\h7_usb.bin" demo.bin
pause //页面暂停
