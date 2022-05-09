//
// ZIP library example sketch
// Based on https://github.com/bitbank2/unzipLIB
//

#include <SPIFFS.h>
#include <fstream>
#include <unzipLIB.h>
// Use a zip file in memory for this test
#include "bmp_icons.h"

void unzipTest();

void task(){
  unzipTest();
  Serial.println("************************************");
  Serial.flush();
Serial.printf_P(PSTR("<<<<<<< free heap memory *3* %d >>>>>>>>\n"), ESP.getFreeHeap());
  while (1)
  {
          vTaskDelay( 1 * portTICK_PERIOD_MS ); // actually 150 ms period, why??
  }
  

}

static File myfile;

//
// Callback functions needed by the unzipLIB to access a file system
// The library has built-in code for memory-to-memory transfers, but needs
// these callback functions to allow using other storage media
//
void * myOpen(const char *filename, int32_t *size) {
  myfile = SPIFFS.open(filename);
  *size = myfile.size();
  return (void *)&myfile;
}
void myClose(void *p) {
  ZIPFILE *pzf = (ZIPFILE *)p;
  File *f = (File *)pzf->fHandle;
  if (f) f->close();
}

int32_t myRead(void *p, uint8_t *buffer, int32_t length) {
  ZIPFILE *pzf = (ZIPFILE *)p;
  File *f = (File *)pzf->fHandle;
  return f->read(buffer, length);
}

int32_t mySeek(void *p, int32_t position, int iType) {
  ZIPFILE *pzf = (ZIPFILE *)p;
  File *f = (File *)pzf->fHandle;
  if (iType == SEEK_SET)
    return f->seek(position);
  else if (iType == SEEK_END) {
    return f->seek(position + pzf->iSize); 
  } else { // SEEK_CUR
    long l = f->position();
    return f->seek(l + position);
  }
}

void listAllFiles(){
   File root = SPIFFS.open("/");
   File file = root.openNextFile();
   while(file){
       Serial.print("FILE: ");
      Serial.println(file.name());
 
      file = root.openNextFile();
  }
}

void spiffs_write_read_test()
{

  // bool formatted = SPIFFS.format();
  //  if(formatted){
  //   Serial.println("\n\nSuccess formatting");
  // }else{
  //   Serial.println("\n\nError formatting");
  // }
 

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
   Serial.println("\n\n----Listing files after format----");
  listAllFiles();

  std::ofstream fileToWrite;
  fileToWrite.open("/spiffs/test.txt");
   
  fileToWrite << "Test";
  fileToWrite.close();
 
  std::ifstream fileToRead("/spiffs/test.txt");
 
  char c;
  while(fileToRead.get(c)){
 
    Serial.print(c);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {};

  spiffs_write_read_test();

Serial.printf_P(PSTR("<<<<<<< free heap memory *1* %d >>>>>>>>\n"), ESP.getFreeHeap());

      TaskHandle_t xHandle = NULL;


      xTaskCreatePinnedToCore(
                (TaskFunction_t)task,   /* Function to implement the task */
                "task", /* Name of the task */
                50000,      /* Stack size in words */
                NULL,       /* Task input parameter */
                0,          /* Priority of the task */
                &xHandle,       /* Task handle. */
                1);  /* Core where the task should run */

  delay(1000);
  Serial.println("UNZIP library demo - open a zip file from FLASH");
  Serial.flush();
  if( xHandle != NULL )
     {
         vTaskDelete( xHandle );
     }
Serial.printf_P(PSTR("<<<<<<< free heap memory *2* %d >>>>>>>>\n"), ESP.getFreeHeap());

}

void unzipTest() {
  int rc;
  char szComment[256], szName[256];
  unz_file_info fi;
  UNZIP zip; // Statically allocate the 41K UNZIP class/structure

#ifdef ZIP_FILE_AT_ROM
  rc = zip.openZIP((uint8_t *)bmp_icons, sizeof(bmp_icons));
#else
  // file at SPIFFS. To store icons.zip file at flash spiffs let use Arduino IDE plugin
  // from https://github.com/me-no-dev/arduino-esp32fs-plugin
  rc = zip.openZIP("/icons.zip", myOpen, myClose, myRead, mySeek);
#endif

  if (rc == UNZ_OK) {
     rc = zip.getGlobalComment(szComment, sizeof(szComment));
     Serial.print("Global comment: ");
     Serial.println(szComment);
     Serial.println("Files in this archive:");
     zip.gotoFirstFile();

      uint8_t *ucBitmap; // temp storage for each icon bitmap 
      rc = zip.getFileInfo(&fi, szName, sizeof(szName), NULL, 0, szComment, sizeof(szComment));
      if (rc == UNZ_OK) {
        ucBitmap = (uint8_t *)malloc(fi.uncompressed_size); // allocate enough to hold the bitmap
          if (ucBitmap != NULL) { // malloc succeeded (it should, these bitmaps are only 2K bytes each)
            zip.openCurrentFile(); // if you don't open it explicitly, readCurrentFile will fail with UNZ_PARAMERROR
            rc = zip.readCurrentFile(ucBitmap, fi.uncompressed_size); // we know the uncompressed size of these BMP images
            for(int i=0; i<fi.uncompressed_size;i++){
              Serial.print(ucBitmap[i], HEX);
              Serial.print(" ");
            }
          }
      }
     rc = UNZ_OK;
     while (rc == UNZ_OK) { // Display all files contained in the archive
        rc = zip.getFileInfo(&fi, szName, sizeof(szName), NULL, 0, szComment, sizeof(szComment));
        if (rc == UNZ_OK) {
          Serial.print(szName);
          Serial.print(" - ");
          Serial.print(fi.compressed_size, DEC);
          Serial.print("/");
          Serial.println(fi.uncompressed_size, DEC);
        }
        rc = zip.gotoNextFile();
     }
     zip.closeZIP();
  }
}

void loop(){

}
