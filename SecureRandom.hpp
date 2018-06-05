#ifndef __SecureRandom_hpp_2018_05_31_10_48
#define __SecureRandom_hpp_2018_05_31_10_48
#include <Random.hpp>

/*
    The SecureRandom class provides a cryptographically secure psuedo-random number generator fit for purposes
    where Secure Random bytes are required. It is greatly advised that you use this class for Cryptographic Purposes
    over the Random Class.
    Because SecureRandom inherits from Random, all Psuedo-random generator methods provided by that class are
    inherited
*/
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
    /*
        Constructs a new Cryptographically Secure Pseudorandom Number generator.
    */
	SecureRandom();
    /*
        Sets the State of this cryptographically secure Pseudorandom Number generator by deriving from 
        a 64-bit value. Unless Predetermined Seeding is required, this method should be avoided.
    */
	void setSeed(uint64_t);
    /*
        Causes the Cryptographically Secure Psuedorandom Number generator to seed itself in an implementation defined way.
        The resulting state is guarenteed (with an extremely high probability) of being different from any other
        produce by annother invocation of this method.
    */
	void seed();
    /*
        Fills an array with a requested ammount of Cryptographicall Secure Pseudorandom bytes.
        If the generator has not be seeded either by calling the setSeed or the seed method,
        the function will seed itself as though by a call to seed().
        The resultant state of the pseudorandom number generator should not be corelatable at all to the output
        of this function.
    */
	void nextBytes(uint8_t*,size_t)override;
};

#endif
