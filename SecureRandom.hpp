#ifndef __SecureRandom_hpp_2018_05_31_10_48
#define __SecureRandom_hpp_2018_05_31_10_48
#include <Random.hpp>

class SecureRandom : public Random{
private:
	char state[32];
	char secondary[32];
	bool seeded;
	mutex lock;
	void initSeed(const void*,size_t);
	void update(char (&)[32]);
protected:
	uint32_t next(int bits);
public:
	SecureRandom();
	void setSeed(uint64_t);
	void seed();
	void nextBytes(uint8_t*,size_t)override;
};

#endif