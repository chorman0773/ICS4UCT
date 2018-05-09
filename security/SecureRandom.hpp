#ifndef __SecureRandom_hpp_2018_05_09_17_44
#define __SecureRandom_hpp_2018_05_09_17_44

#include <Random.hpp>
#include <cstdint>

class SecureRandom: final Random{
private:
  unsigned char key1[32];
  unsigned char key2[32];
  unsigned char state[32];
  unsigned char iv[16];
  bool seeded;
  void initSeed(uint8_t*,uint32_t);
protected:
  unsigned int next(unsigned int);
public:
  SecureRandom();
  void setSeed(uint64_t);
  void seed();
  void nextBytes(uint8_t*,uint32_t);
};

#endif
