#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

void printBinary ( size_t b ) {
  if (b == 0) {
    printf("0b0\n");
    return;
  }
  char str[67] = {0};
  str[0] = '0';
  str[1] = 'b';

  size_t mask = 1ULL<<63;
  size_t index = 2;
  do {
    if (mask & b) str[index] = '1';
    else str[index] = '0';
    index++;
    mask = mask>>1;
  }  while (mask != 0);
  printf("%s\n", str);
}

int main ()  {

  char b = 15;
  // uint32_t htonl(uint32_t hostlong);
  uint32_t htonlInt = htonl(b);
  printf("htonlInt : %d\n",htonlInt);
  printBinary(htonlInt);


  // uint16_t htons(uint16_t hostshort);
  uint16_t htonsInt = htons(b);
  printf("htonsInt : %d\n", htonsInt);
  printBinary(htonsInt);


  //  uint32_t ntohl(uint32_t netlong);
  uint32_t ntohlInt = ntohl(b);
  printf("ntohlInt : %d\n", ntohlInt);
  printBinary(ntohlInt);

  // uint16_t ntohs(uint16_t netshort);
  uint16_t ntohsInt = ntohs(b);
  printf("ntohsInt : %d\n", ntohsInt);
  printBinary(ntohsInt);


  uint32_t test = ntohl(htonlInt);
    printf("test : %d\n", test);
  printBinary(test);


  return 0;
}