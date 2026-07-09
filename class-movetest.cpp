#include "ihm02a1.h"
#include <unistd.h>
#define ms 1000

int main(){

  ihm02a1* board;
  board = new ihm02a1("/dev/spidev0.0", 2);
//printf("%d\n", board->numDevices);

  uint8_t message[] = {0xc0, 0xd0, 0x00, 0x00};
  uint8_t len = sizeof(message);
  uint8_t tx_buffer[32];
  uint8_t rx_buffer[32];
  
  memset(tx_buffer, 0xff, 32);
  memset(rx_buffer, 0xff, 32);

  board->interlace(tx_buffer, message, len, 0b11);
  board->writeRead(rx_buffer, tx_buffer, len);

  usleep(1000*ms);

  uint8_t jog[] = {0x51, 0x01, 0x00, 0x00};
  memcpy(message, jog, 4);

  board->interlace(tx_buffer, message, len, 0b10);
  board->writeRead(rx_buffer, tx_buffer, len);

  usleep(500*ms);

  message[0] = 0xb0;
  board->interlace(tx_buffer, message, 1, 0b11);
  board->writeRead(rx_buffer, tx_buffer, 1);

  usleep(1500*ms);

  message[0] = 0x70;
  board->interlace(tx_buffer, message, 1, 0b10);
  board->writeRead(rx_buffer, tx_buffer, 1);

//hexdump
  for(int i = 0; i < 32; i++){
    printf("%02x ", rx_buffer[i]);
    if(i%8 == 7) printf("\n");
  }
  
  return 0;
}

