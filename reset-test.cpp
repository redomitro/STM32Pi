#include "ihm02a1.h"
#include <unistd.h>
#define ms 1000

int main(){

  ihm02a1* board;
  board = new ihm02a1("/dev/spidev0.0", 2);

  board->reset(0b11);
  
  usleep(500*ms);
  uint8_t status[32] = {0};
  
  board->getStatus(status, 0b11);

  for (int i = 0; i < 32; i++){
    printf("%02x ", status[i]);
    if((i%8)==7) printf("\n");
  }
  return 1;
}
