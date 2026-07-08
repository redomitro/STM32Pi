#include "ihm02a1.h"

int main(){

  ihm02a1* board;
  board = new ihm02a1("/dev/spidev0.0", 2);
  printf("%d\n", board->numDevices);
  /*
  uint8_t message[] = {1, 2, 3, 4, 65, 66, 67, 68};
  uint8_t tx_buffer[32];

  memset(tx_buffer, 0xff, 32);

  interlace(tx_buffer, message, 4, 0b01, 2);
  interlace(tx_buffer, message+4, 4,  0b10, 2);
  
  for(int i = 0; i < 32; i++){
    printf("%02x ", tx_buffer[i]);
    if(i%8 == 7) printf("\n");
  }
  */
  return 0;
}

