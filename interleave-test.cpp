#include "ihm02a1.h"

int main(){

  ihm02a1* board;
  board = new ihm02a1("/dev/spidev0.0", 2);
//printf("%d\n", board->numDevices);

  uint8_t len = 3;
  uint8_t message[] = {0x38, 0x00, 0x00};
  uint8_t tx_buffer[32];
  uint8_t rx_buffer[32];
  
  memset(tx_buffer, 0xff, 32);
  memset(rx_buffer, 0xff, 32);

  board->interlace(tx_buffer, message, len, 0b11);
  board->writeRead(rx_buffer, tx_buffer, len);

//hexdump
  for(int i = 0; i < 32; i++){
    printf("%02x ", rx_buffer[i]);
    if(i%8 == 7) printf("\n");
  }
  
  return 0;
}

