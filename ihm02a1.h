#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <spidev_lib++.h>

/* I want to do this with classes */

class ihm02a1{
public:
  ihm02a1(const char* spiDev, uint8_t numDevs);
  //methods follow the memcpy convention of (dest, source, len, [mask])
  uint8_t interlace(uint8_t  *output, uint8_t *input, uint8_t len, uint8_t mask);
  uint8_t deinterlace(uint8_t *output, uint8_t *input, uint8_t len);
  uint8_t writeRead(uint8_t* rx, uint8_t* tx, uint8_t len);
  uint8_t numDevices;

private:
  spi_config_t spiConfig;
  SPI* sB; //pointer to this board's SPI bus. Abbreviated because it is used very frequently
};

ihm02a1::ihm02a1(const char* spiDev, uint8_t numDevs){
  spiConfig.mode = 0;
  spiConfig.speed = 115200;
  spiConfig.delay = 0;
  spiConfig.bits_per_word = 0;

  sB = new SPI(spiDev, &spiConfig);
  if(numDevs > 8){
    printf("Too many devices, communication will fail!\n");
  }
  sB->begin();
  numDevices = numDevs;
}

uint8_t ihm02a1::interlace(uint8_t *output, uint8_t *input, uint8_t len, uint8_t mask){

  if(sizeof(output) < len*numDevices){
    std::cout << "Interlace: Invalid output\n";
    return -1;
  }

  for(uint8_t i = 0; i < numDevices; i++){

  //order of device; remember device directly connected to host MOSI
  //is last in chain and corresponds to mask msb. I love SPI too

    if((mask >> i) & 1){ //extract corresponding mask bit with bitwise ops

      for(uint8_t j = 0; j < len; j++){
        memcpy(output+i+j*numDevices, input+j, 1);

        /* if 4 devices connected:
        device 0 will read from bits 0, 4, 8, 12 etc
        device 1 will read from bits 1, 5, 9, 13
        etc */
      }
    }
  }

  return 1;
}

uint8_t ihm02a1::writeRead(uint8_t* rx, uint8_t* tx, uint8_t len){

  if(sizeof(rx) < len*numDevices){
    std::cout << "WR: Invalid output\n";
    return -1;
  }

  for(int i = 0; i < len; i++){
    sB->xfer(tx+i*numDevices, numDevices, rx+i*numDevices, numDevices);
  }

  return 1;
}

uint8_t ihm02a1::deinterlace(uint8_t *output, uint8_t *input, uint8_t len){

  if(sizeof(output) > len*numDevices){
    std::cout << "Deinterlace: Invalid output\n";
    return -1;
  }

  if(numDevices > 8){
    /*  ok to hardcode this since the ihm02a1
    only supports daisy chaining up to 4 boards
    (8 total controllers) */
    std::cout << "Too many devices\n";
    return -1;
  }

  for(uint8_t i = 0; i < numDevices; i++){
  //order of devices; device connected directly to MISO (last in wire chain) is first in data chain

    for (uint8_t j = 0; j < len; j++){
      memcpy(output+j+i*len, input+j*len+i, 1); //basically transposing a matrix here
    }
  }
  return 1;
}
