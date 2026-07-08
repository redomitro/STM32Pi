#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spidev_lib++.h>

spi_config_t spi_config;
uint8_t tx_buffer[32];
uint8_t rx_buffer[32];
uint8_t zero[32]={0};

int  main( void)
{

  SPI *mySPI = NULL;

  spi_config.mode=0;
  spi_config.speed=1000000;
  spi_config.delay=0;
  spi_config.bits_per_word=8;

  mySPI=new SPI("/dev/spidev0.0",&spi_config);

  if (!mySPI->begin())
    return -1;
  memset(tx_buffer,0,32);
  memset(rx_buffer,0,32);

  /*
  mySPI->xfer(tx_buffer,strlen((char*)tx_buffer),rx_buffer,strlen((char*)tx_buffer));
  */

  for (int i=0; i<5; i++){
    mySPI->xfer(tx_buffer+2*i, 2, rx_buffer+2*i, 2);
  }

  for(int i = 0; i<4; i++){
    for(int j = 0; j<8; j++){
      printf("%02x ", (char)rx_buffer[8*i+j]);
  }
    printf("\n");
  }
    
  delete mySPI; 

 return 1;
}
