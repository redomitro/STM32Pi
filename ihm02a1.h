#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <spidev_lib++.h>

/*
#define NOP 0x00
#define ABS_POS 0x01
#define EL_POS 0x02
#define MARK 0x03
#define SPEED 0x04
#define ACC 0x05
*/

#define NOP 0x00
#define GO_HOME 0x70
#define GO_MARK 0x78
#define SOFT_HIZ 0xa0
#define HARD_HIZ 0xa8
#define SOFT_STOP 0xb0
#define HARD_STOP 0xb8
#define RESET_DEVICE 0xc0
#define RESET_POS 0xd8

class ihm02a1{
public:
  ihm02a1(const char* spiDev, uint8_t numDevs);
  //methods follow the memcpy convention of (dest, source, len, [mask])
  uint8_t interlace(uint8_t  *output, uint8_t *input, uint8_t len, uint8_t mask);
  uint8_t deinterlace(uint8_t *output, uint8_t *input, uint8_t len);
  uint8_t writeRead(uint8_t* rx, uint8_t* tx, uint8_t len);
  uint8_t numDevices;

  //functions that call command
  uint8_t reset(uint8_t mask);
  uint8_t goHome(uint8_t mask);
  uint8_t goMark(uint8_t mask);
  uint8_t resetPos(uint8_t mask);
  uint8_t softStop(uint8_t mask);
  uint8_t softHiZ(uint8_t mask);
  uint8_t hardStop(uint8_t mask);
  uint8_t hardHiZ(uint8_t mask);

  //functions that call commandResponse
  uint8_t getStatus(uint8_t* output, uint8_t mask);
  uint8_t getParam(uint8_t* output, uint8_t param, uint8_t mask);

  //functions that call commandArg
  uint8_t setParam(uint8_t param, size_t value, uint8_t mask);
  uint8_t jog(bool dir, uint8_t speed, uint8_t mask);
  uint8_t tweak(bool dir, uint8_t distance, uint8_t mask);
  uint8_t move(uint8_t pos, uint8_t mask);

private:
  spi_config_t spiConfig;
  uint8_t command(uint8_t cmd, uint8_t mask);
  uint8_t commandArg(uint8_t cmd, size_t arg, uint8_t argLen, uint8_t mask);
  uint8_t commandResponse(uint8_t* output, uint8_t cmd, uint8_t respLen, uint8_t mask);
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

  if(sizeof(output) < len*numDevices){
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

uint8_t ihm02a1::command(uint8_t cmd, uint8_t mask){
  //generic function for all one-byte commands
  uint8_t message[1] = {cmd};
  uint8_t txBuffer[numDevices] = {0};
  uint8_t rxBuffer[numDevices] = {0};

  this->interlace(txBuffer, message, 1, mask);
  this->writeRead(rxBuffer, txBuffer, 1);
  return 1;
}

uint8_t ihm02a1::reset(uint8_t mask){
  return this->command(RESET_DEVICE, mask);
}

uint8_t ihm02a1::goHome(uint8_t mask){
  return this->command(GO_HOME, mask);
}

uint8_t ihm02a1::goMark(uint8_t mask){
  return this->command(GO_MARK, mask);
}

uint8_t ihm02a1::resetPos(uint8_t mask){
  return this->command(RESET_POSITION, mask);
}

uint8_t ihm02a1::softStop(uint8_t mask){
  return this->command(SOFT_STOP, mask);
}

uint8_t ihm02a1::softHiZ(uint8_t mask){
  return this->command(SOFT_HIZ, mask);
}

uint8_t ihm02a1::hardStop(uint8_t mask){
  return this->command(HARD_STOP, mask);
}

uint8_t ihm02a1::hardHiZ(uint8_t mask){
  return this->command(HARD_HIZ, mask);
}

uint8_t ihm02a1::getStatus(uint8_t* output, uint8_t mask){

  uint8_t message[] = {0xd0, 0x00, 0x00};
  uint8_t tx_buffer[32] = {0};
  uint8_t rx_buffer[32] = {0};

  interlace(tx_buffer, message, 3, mask);
  writeRead(rx_buffer, tx_buffer, 3);

  deinterlace(output, rx_buffer+numDevices, 2);
  //discards the first returned word which should be 0x00 anyway
  return 1;
}
