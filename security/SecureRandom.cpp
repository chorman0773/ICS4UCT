#include "SecureRandom.hpp"
#include <openssl/AES.h>
#include <openssl/SHA.h>

#ifdef __WIN32
#define _CRT_RAND_S
#include <stdlib.h>
#else
#include <cstdio>
#endif
void fillRandomBytes(uint8_t* bytes,uint32_t len){
#ifdef __WIN32
  uint32_t* buff = (uint32_t*)bytes;
  for(int i = 0;i<len;i+=4)
    rand_s(buff+i);
#else
  FILE* f = fopen("/dev/urandom");
  fread(bytes,len,1,f);
  fclose(f);
#endif
}

uint64_t rot(uint64_t r,int val){
  return r>>val|(r<<(64-val));
}

char staticIv[] = "\xEB\xDD\xA5\x8B\xBB\x88\xCF\x0E\x0D\x0C\xFD\x42\xCD\x47\x8A\xCB";

void reduce(uint8_t* inBuffer,uint8_t* outBuffer,uint32_t outLen){
  for(int i = 0;i<outLen;i++)
    outBuffer[i] = inBuffer[2*i]^inBuffer[2*i+1];
}

SecureRandom::SecureRandom(){
  seeded = false;
}

void SecureRandom::initSeed(uint8_t* seed,uint32_t length){
  uint8_t buffer[64];
  AESKEY key;
  SHA256(seed,length,this->state);
  SHA512(seed,length,buffer);
  reduce(buffer,this->key1,32);
  reduce(buffer,buffer,32);
  SHA256(buffer,32,this->key2);
  SHA256(seed,length,buffer);
  AES_set_encrypt_key(this->key1,256,&key);
  AES_cbc_encrypt(buffer,buffer,32,&key,staticIv,1);
  reduce(buffer,this->iv,16);
  seeded = true;
}

void SecureRandom::setSeed(uint64_t seed){
  uint64_t expanded[32];
  expand[0] = seed;
  expand[1] = rot(seed,31)^rot(seed,13);
  expand[2] = rot(seed,31)^rot(seed,13)^rot(expand[1],47);
  for(int i = 3;i<sizeof(expanded)/sizeof(uint64_t);i++)
    expand[i] = rot(seed,31)^rot(expanded[i-1],13)^rot(expanded[i-2],47);
   initSeed((uint8_t*)expanded,sizeof(expanded));
}

void SecureRandom::seed(){
  uint8_t data[128];
  fillRandomBytes(data,sizeof(data));
  initSeed(data,sizeof(data));
}

void SecureRandom::nextBytes(uint8_t* out,size_t size){
  if(!seeded)
    seed();
   
}
