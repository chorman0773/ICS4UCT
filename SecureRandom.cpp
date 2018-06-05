#include <SHA256.hpp>
#include <random>
#include <SecureRandom.hpp>

int h[8] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5};

/*
	Implementation for SecureRandom.hpp
	The SecureRandom class provides Cryptographically Secure Pseudo-random Numbers
	via a transformed multihash deterministic algorithm.
	The class provides the guarentee that the output cannot be co-related to the previous state
	The same Thread Guarentee as the Random class is provided by the Secure Random Class.
*/

/*
	Applys the rightrotate operation on val.
	n rightrotate v 
	for v>=32 is the same as n rightrotate v%32
	And n rightrotate v is equivalent to n>>v|n<<(32-v)
	This function is a psuedo-implementation of the circular-rightshift operation,
	which shifts all the bits right, shifting in the rightmost bits from the previous number into the leftmost position.
*/
int rotate(int val,int by){
  by &=31;
  return val>>(by)|val<<(32-by);
}

/*
	Reduces a given block of bytes by half its size, using xor reduction.
	After the operation, the nth byte in the output is equal to the qth byte in the input xor the qth+1 byte in the input,
	where q is equal to 2*n.
	The output and input are combined for efficiency.
*/
void reduce(char* block,size_t inSize){
	for(size_t i = 0;i<inSize/2;i++)
		block[i] = block[2*i]^block[2*i+1];
}



using std::random_device;


/*
	Obtains the nth transformation byte.
	This value is derived from the hash constants used for seed expansion
	Obtaining the 31st+n transformation byte is the same as obtaining the nth transformation byte.
*/
char getTransformByte(size_t pos){
	int h[8] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5};
	pos = pos%31;
	return (char)(rotate(h[0],pos*11)^rotate(h[1],pos*13)^rotate(h[2],pos*7)^rotate(h[3],pos*17)
		^rotate(h[4],pos*19)^rotate(h[5],pos*23)^rotate(h[6],pos*29)^rotate(h[7],pos*31));
}
/*
	Obtains the nth transformation word.
	This value is derived from the hash constants used for seed expansion.
	Obtaining the 29th+n transformation word is the same as obtaining the nth transformation word.
*/
int getTransformWord(size_t pos){
	int h[8] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5};
	pos = pos%29;
	return (rotate(h[0],pos*11)^rotate(h[1],pos*13)^rotate(h[2],pos*7)^rotate(h[3],pos*17)
		^rotate(h[4],pos*19)^rotate(h[5],pos*23)^rotate(h[6],pos*29)^rotate(h[7],pos*31));
}

/*
	Automatically seeds this CSPNRG from a strong random number source.
	This can be done by calling seed on an instance of SecureRandom,
	or is done automatically if you attempt to aquire random numbers from the generator
	before it has been seeded. 
	This function is designed to guarentee that 2 invocations of this function will never generate the same state
	(with about a 2^-512 chance that it will not.)
	This function should be used if and only if you need to reinitialize the pseudo-random state,
	and do not need to make this initialization consistent with other sources.
	Otherwise, you should either initialize the CSPRNG with a strong seed, or should let it self-initialize.
	The former is only to be used in the case where you need consistant results.
	Otherwise you should only seed an Instance of SecureRandom via this method.
*/
void SecureRandom::seed(){
	
	int words[64] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19,
          0xc1059ed8,0x367cd507,0x3070dd17,0xf70e5939,0xffc00b31,0x68581511,0x64f98fa7,0xbefa4fa4,
          0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
           0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
           0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
           0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
           0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
           0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
   random_device dev;
	for(int q = 0;q<16;q++){
		int a = h[0];
		int b = h[1];
		int c = h[2];
		int d = h[3];
		int e = h[4];
		int f = h[5];
		int g = h[6];
		int j = h[7];

    
		for(int i = 0;i<16;i++)
		  words[i] ^= dev();
		for(int i = 16;i<64;i++){
		  int val1 = rotate(words[i-15],5)^rotate(words[i-16],7)^rotate(words[i-15],17)^0xB7E15162;
		  int val2 = rotate(words[i-7],13)^rotate(words[i-8],19)^rotate(words[i-7],11)^0x243F6A88;
		  int tmp1 = (rotate(e,7)^rotate(f,11)^rotate(g,13)^j)+val1;
		  int tmp2 = (rotate(a,23)^rotate(b,7)^rotate(c,29)^d)+val2;
		  j = g;
		  g = f;
		  f = e;
		  e = d+tmp1;
		  d = c;
		  c = b;
		  b = a;
		  a = tmp1+tmp2;
		  words[i] += val1+val2+words[i-16]+0x9E3779B9;
		}
		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
		h[4] += e;
		h[5] += f;
		h[6] += g;
		h[7] += j;
	}
    initSeed(&words,sizeof(words));
}

/*
	Helper function which initializes the state of the csprng.
	This function is called by the seed function and the setSeed function.
*/
void SecureRandom::initSeed(const void* v,size_t s){
	const char* c = (const char*)v;
	char* q = new char[s];
	
	for(size_t i = 0;i<s;i++)
		q[i] = c[i]^getTransformByte(i);
	SHA256(c,s,this->state);
	SHA256(q,s,this->secondary);
	delete[] q;
	seeded = true;
}


void SecureRandom::update(char (&bytes)[32]){
	char q[32];
	char v[32];
	for(size_t i = 0;i<32;i++)
		q[i] = secondary[i]^getTransformByte(i);
	for(size_t i = 0;i<32;i++)
		v[i] = state[i]^getTransformByte(i);
	SHA256(state,32,bytes);
	SHA256(v,32,secondary);
	SHA256(q,32,state);
}

void SecureRandom::nextBytes(uint8_t* out,size_t size){
	lock.lock();
	size_t tSize = size;
	if(!seeded)seed();
	while((tSize%32)!=0)tSize*=2;
	char* tOut = new char[tSize];
	char (*byteBlocks)[32] = reinterpret_cast<char(*)[32]>(tOut);
	size_t blocksC = tSize/32;
	for(size_t i = 0;i<blocksC;i++)
		update(byteBlocks[i]);
	while(tSize!=size){
		tSize/=2;
		reduce(tOut,tSize);
	}
	memcpy(out,tOut,size);
	lock.unlock();
}

uint32_t SecureRandom::next(int bits){
	
	union{
		uint32_t val;
		uint8_t bytes[4];
	};
	nextBytes(bytes,4);
	return val>>(32-bits);
}

SecureRandom::SecureRandom():seeded(false),Random(){}


void SecureRandom::setSeed(uint64_t seed){
	int h[8] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5};
	this->Random::setSeed(seed);//To clear haveNextNextGaussian
	int high = int(seed>>32),low = int(seed);
	int words[64] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19,
          0xc1059ed8,0x367cd507,0x3070dd17,0xf70e5939,0xffc00b31,0x68581511,0x64f98fa7,0xbefa4fa4,
          0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
           0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
           0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
           0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
           0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
           0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
	int a = h[0];
    int b = h[1];
    int c = h[2];
    int d = h[3];
    int e = h[4];
    int f = h[5];
    int g = h[6];
    int j = h[7];
	for(size_t i = 0;i<16;i++)
		words[i] ^= rotate(high,13*i)^rotate(low,11*i)^getTransformWord(i);
	for(size_t i = 16;i<64;i++){
      int val1 = rotate(words[i-15],5)^rotate(words[i-16],7)^rotate(words[i-15],17)^0xB7E15162;
      int val2 = rotate(words[i-7],13)^rotate(words[i-8],19)^rotate(words[i-7],11)^0x243F6A88;
      int tmp1 = (rotate(e,7)^rotate(f,11)^rotate(g,13)^j)+val1;
      int tmp2 = (rotate(a,23)^rotate(b,7)^rotate(c,29)^d)+val2;
      j = g;
      g = f;
      f = e;
      e = d+tmp1;
      d = c;
      c = b;
      b = a;
      a = tmp1+tmp2;
      words[i] += val1+val2+words[i-16]+0x9E3779B9;
    }
}