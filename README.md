# esp32_unzip_test

Input data from **bmp_icons.h**

Output to RAM variable **ucBitmap**

Library does not use malloc/free, but use class instans 41 KByte (at stack)

Need to create task with large stack size - 50 Kbyte then perfomr unzip and then this task deleted