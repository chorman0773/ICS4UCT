#include "Random.hpp"
#include <ctime>
#include <chrono>
#include "JTime.hpp"
seed_t number(32);
const seed_t cprime = 4989641;

seed_t highResTime(){
	Instant i = Instant::now();
	return i.get(ChronoUnit::MILISECONDS);
}

seed_t nextMultiplier(){
	seed_t val = number;
	number*=2;
	if(val%10==2)
		val--;//Adjacent prime is likely 1 less than val, if it exists
	else if(val%10==4)
		return nextMultiplier();//Probably doesn't have an adjacent prime
	else if(val%10==6)
		val++;//Adjacent prime is likely 1 more than val, if it exists
	else if(val%8==8)
		return nextMultiplier();//Probably doesn't have an adjacent prime
	return val*cprime;
}
seed_t genUniqueSeed(){
	return highResTime()*nextMultiplier();
}
seed_t initRandomizeSeed(seed_t seed){
	return (seed ^ 0x5DEECE66DL) & ((1L << 48) - 1);
}

uint32_t Random::next(int bits){
	int ret;
	lock.lock();
	seed = (seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
	ret = (int)(seed >> (48 - bits));
	lock.unlock();
	return ret;
}
Random::Random():lock(){
	setSeed(genUniqueSeed());
	
}
Random::Random(seed_t s):lock(){
	setSeed(s);
}
void Random::setSeed(seed_t seed){
	lock.lock();
	this->seed = initRandomizeSeed(seed);
	this->haveNextNextGaussian = false;
	lock.unlock();
}

int Random::nextInt(){
	return next(31);
}
int Random::nextInt(int bound){
	if(bound<=0)
		throw "Bound must greater then 0";

	if ((bound & -bound) == bound)  // i.e., bound is a power of 2
		return (int)((bound * (seed_t)next(31)) >> 31);

	int bits, val;
	do {
		bits = next(31);
		val = bits % bound;
	} while (bits - val + (bound-1) < 0);
		return val;
}

double Random::nextGuassian(){
	double ret;
	lock.lock();
	if (haveNextNextGaussian) {
			haveNextNextGaussian = false;
			ret = nextNextGaussian;
	} else {
			double v1, v2, s;
			do {
			v1 = 2 * nextDouble() - 1;   // between -1.0 and 1.0
			v2 = 2 * nextDouble() - 1;   // between -1.0 and 1.0
			s = v1 * v1 + v2 * v2;
			} while (s >= 1 || s == 0);
			double multiplier = sqrt(-2 * log(s)/s);
			nextNextGaussian = v2 * multiplier;
			haveNextNextGaussian = true;
			ret = v1 * multiplier;
		}
	lock.unlock();
	return ret;
}
int64_t Random::nextLong(){
	return nextInt()<<32LL+nextInt();
}

float Random::nextFloat(){
	return  next(24) / ((float)(1 << 24));
}

double Random::nextDouble(){
	return ((long)next(26) << 27 + next(27))/((double)(1L << 53));
}

void Random::nextBytes(uint8_t* out,size_t size){
	for (int i = 0; i < size; )
		for (int rnd = nextInt(), n = __min(size - i, 4);
        	  n-- > 0; rnd >>= 8)
      		 bytes[i++] = (byte)rnd;
}


