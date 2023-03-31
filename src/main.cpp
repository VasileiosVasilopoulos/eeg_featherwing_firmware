#include <Arduino.h>
#include <board_definitions.h>
#include <board_functions.h>


GEENIE Geenie;

void setup() {
  // put your setup code here, to run once:
  delay(5000);
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Ready");
  Geenie.set_buttons();
  Geenie.initialize();
  // byte ads_addr = Geenie.read_ads();

  Geenie.activateChannel(1, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(2, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(3, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(4, ADS_GAIN24, ADSINPUT_NORMAL);
  
  delay(4000);
  Geenie.RREGS(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4

  Geenie.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Geenie.streaming) {
    if (Geenie.channelDataAvailable) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      Geenie.updateChannelData();
      Geenie.sendChannelDataSerial(Geenie.PACKET_TYPE_ACCEL);
    }
  }
}


// #include <ads1299.h>

// ADS1299 ADS;                           // create an instance of ADS1299

// unsigned long thisTime;                
// unsigned long thatTime;
// unsigned long elapsedTime;
// int resetPin = 9;                      // pin 9 used to start conversion in Read Data Continuous mode
// int sampleCounter = 0;                 // used to time the tesing loop
// boolean testing = true;               // this flag is set in serialEvent on reciept of prompt


// void serialEvent(){            // send an 'x' on the serial line to trigger ADStest()
//   while(Serial.available()){      
//     char inChar = (char)Serial.read();
//     if (inChar  == 'x'){   
//       testing = true;
//     }
//   }
// }

// void ADStest(int numSamples){
//   while(sampleCounter < numSamples){  // take only as many samples as you need
//     while(digitalRead(ADS_DRDY)){            // watch the DRDY pin
//       }
//     ADS.RDATA();          // update the channelData array 
//     ADS.printChannelDataAsText(8, 0);
//     sampleCounter++;                  // increment sample counter for next time
//   }
//     return;
// }


// void setup() {
//   // don't put anything before the initialization routine for recommended POR  
//   ADS.initialize(BOARD_ADS, false, true); // (DRDY pin, RST pin, CS pin, SCK frequency in MHz);

//   Serial.begin(115200);
//   Serial.println("ADS1299-Arduino UNO Example 2"); 
//   delay(2000);             

//   ADS.verbose = true;      // when verbose is true, there will be Serial feedback 
//   ADS.RESET();             // send RESET command to default all registers
//   ADS.SDATAC();            // exit Read Data Continuous mode to communicate with ADS
//   ADS.RREGS(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4
//   ADS.WREG(CONFIG3,0xE0);  // enable internal reference buffer, for fun
//   ADS.RREG(CONFIG3);       // verify write
//   ADS.WREG(CH1SET,0b01100000);  // enable internal reference buffer, for fun
//   ADS.RREG(CH1SET);       // verify write
//   ADS.WREG(CONFIG4,0b00001000);  // enable internal reference buffer, for fun
//   ADS.RREG(CONFIG4);       // verify write


//   // ADS.RDATAC();            // enter Read Data Continuous mode
  
//   Serial.println("Press 'x' to begin test");    // ask for prompt
// } // end of setup

// void loop(){
  
//   if (testing){
//     Serial.println("entering test loop");
//     ADS.START();                    // start sampling at the default rate
//     thatTime = millis();            // timestamp
//     ADStest(50);                   // go to testing routine and specify the number of samples to take
//     thisTime = millis();            // timestamp
//     ADS.STOP();                     // stop the sampling
//     elapsedTime = thisTime - thatTime;
//     Serial.print("Elapsed Time ");Serial.println(elapsedTime);  // benchmark
//       Serial.print("Samples ");Serial.println(sampleCounter);   // 
//     testing = false;                // reset testing flag
//     sampleCounter = 0;              // reset counter
//     Serial.println("Press 'x' to begin test");  // ask for prompt
//   }// end of testing
  
// } // end of loop






















/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
// #include "FS.h"
// #include "SD.h"
// #include "SPI.h"

// void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
//     Serial.printf("Listing directory: %s\n", dirname);

//     File root = fs.open(dirname);
//     if(!root){
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if(!root.isDirectory()){
//         Serial.println("Not a directory");
//         return;
//     }

//     File file = root.openNextFile();
//     while(file){
//         if(file.isDirectory()){
//             Serial.print("  DIR : ");
//             Serial.println(file.name());
//             if(levels){
//                 listDir(fs, file.name(), levels -1);
//             }
//         } else {
//             Serial.print("  FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }

// void createDir(fs::FS &fs, const char * path){
//     Serial.printf("Creating Dir: %s\n", path);
//     if(fs.mkdir(path)){
//         Serial.println("Dir created");
//     } else {
//         Serial.println("mkdir failed");
//     }
// }

// void removeDir(fs::FS &fs, const char * path){
//     Serial.printf("Removing Dir: %s\n", path);
//     if(fs.rmdir(path)){
//         Serial.println("Dir removed");
//     } else {
//         Serial.println("rmdir failed");
//     }
// }

// void readFile(fs::FS &fs, const char * path){
//     Serial.printf("Reading file: %s\n", path);

//     File file = fs.open(path);
//     if(!file){
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     Serial.print("Read from file: ");
//     while(file.available()){
//         Serial.write(file.read());
//     }
//     file.close();
// }

// void writeFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Writing file: %s\n", path);

//     File file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("File written");
//     } else {
//         Serial.println("Write failed");
//     }
//     file.close();
// }

// void appendFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Appending to file: %s\n", path);

//     File file = fs.open(path, FILE_APPEND);
//     if(!file){
//         Serial.println("Failed to open file for appending");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("Message appended");
//     } else {
//         Serial.println("Append failed");
//     }
//     file.close();
// }

// void renameFile(fs::FS &fs, const char * path1, const char * path2){
//     Serial.printf("Renaming file %s to %s\n", path1, path2);
//     if (fs.rename(path1, path2)) {
//         Serial.println("File renamed");
//     } else {
//         Serial.println("Rename failed");
//     }
// }

// void deleteFile(fs::FS &fs, const char * path){
//     Serial.printf("Deleting file: %s\n", path);
//     if(fs.remove(path)){
//         Serial.println("File deleted");
//     } else {
//         Serial.println("Delete failed");
//     }
// }

// void testFileIO(fs::FS &fs, const char * path){
//     File file = fs.open(path);
//     static uint8_t buf[512];
//     size_t len = 0;
//     uint32_t start = millis();
//     uint32_t end = start;
//     if(file){
//         len = file.size();
//         size_t flen = len;
//         start = millis();
//         while(len){
//             size_t toRead = len;
//             if(toRead > 512){
//                 toRead = 512;
//             }
//             file.read(buf, toRead);
//             len -= toRead;
//         }
//         end = millis() - start;
//         Serial.printf("%u bytes read for %u ms\n", flen, end);
//         file.close();
//     } else {
//         Serial.println("Failed to open file for reading");
//     }


//     file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         return;
//     }

//     size_t i;
//     start = millis();
//     for(i=0; i<2048; i++){
//         file.write(buf, 512);
//     }
//     end = millis() - start;
//     Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
//     file.close();
// }

// void setup(){
//     Serial.begin(115200);

//     pinMode(BOARD_ADS, OUTPUT);
//     digitalWrite(BOARD_ADS, HIGH);

//     if(!SD.begin()){
//         Serial.println("Card Mount Failed");
//         return;
//     }
//     uint8_t cardType = SD.cardType();

//     if(cardType == CARD_NONE){
//         Serial.println("No SD card attached");
//         return;
//     }

//     Serial.print("SD Card Type: ");
//     if(cardType == CARD_MMC){
//         Serial.println("MMC");
//     } else if(cardType == CARD_SD){
//         Serial.println("SDSC");
//     } else if(cardType == CARD_SDHC){
//         Serial.println("SDHC");
//     } else {
//         Serial.println("UNKNOWN");
//     }

//     uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//     Serial.printf("SD Card Size: %lluMB\n", cardSize);

//     listDir(SD, "/", 0);
//     createDir(SD, "/mydir");
//     listDir(SD, "/", 0);
//     removeDir(SD, "/mydir");
//     listDir(SD, "/", 2);
//     writeFile(SD, "/hello.txt", "Hello ");
//     appendFile(SD, "/hello.txt", "World!\n");
//     readFile(SD, "/hello.txt");
//     deleteFile(SD, "/foo.txt");
//     renameFile(SD, "/hello.txt", "/foo.txt");
//     readFile(SD, "/foo.txt");
//     testFileIO(SD, "/test.txt");
//     Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
//     Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
// }

// void loop(){

// }