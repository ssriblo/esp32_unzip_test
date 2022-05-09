# esp32_unzip_test

# ZIP data at header file
Input data from **bmp_icons.h**

Output to RAM variable **ucBitmap**

Library does not use malloc/free, but use class instans 41 KByte (at stack)

Need to create task with large stack size - 50 Kbyte then perfomr unzip and then this task deleted

# SPIFFS example
**icon.zip** file placed at **data** folder via Arduino IDE plugin from https://github.com/me-no-dev/arduino-esp32fs-plugin

**ZIP_FILE_AT_ROM** should be commented