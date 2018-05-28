#include "Bindings.hpp"
#include <openssl/rand.hpp>
#include <openssl/sha.hpp>
#include <UUID.hpp>
#include <Hash.hpp>
#include <random>

#include <cstring>

int rotate(int val,int by){
  by &=31;
  return val>>(by)|val<<(32-by);
}

int words[64] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19,
          0xc1059ed8,0x367cd507,0x3070dd17,0xf70e5939,0xffc00b31,0x68581511,0x64f98fa7,0xbefa4fa4,
          0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
           0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
           0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
           0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
           0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
           0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
   int h[8] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5};

/*
  Seed the Random Number Generator that OpenSSL Uses.
  This function is called when there is not enough entropy in the OpenSSL CSPRNG.
  This function preform a SHA-2 like hash expansion and compression with 16 random words from the
  system random device, to an array of 64 words, which is then used to seed the OpenSSL CSPRNG.
  This procedure will repeat until OpenSSL reports that there is enough entropy in its pool.
*/
void seedSystemRandom(){
  
   random_device dev;
  do{
    int a = h[0];
    int b = h[1];
    int c = h[2];
    int d = h[3];
    int e = h[4];
    int f = h[5];
    int g = h[6];
    int j = h[7];

    
    for(int i = 0;<16;i++)
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
    RAND_seed(&words,sizeof(words));
  }while(RAND_status()==0);
}

unsigned char* salt(const unsigned char* pwd,unsigned int size,const unsigned char salt(&)[32]){
  int outSize = size+32-(size%32);
  char* ret = new char[outSize];
  for(int i = 0;i<outSize;i++)
    ret[i] = pwd[i%size]^salt[i%32];
  return ret;
}

Employee::Employee(){}
Employee::Employee(const string& name,const UUID& id,double salary,const unsigned char (&salt)[32],const unsigned char (&hash)[32],
  const EnumSet<Permission>& permissions):name(name),id(id),salary(salary),salt(salt),hash(hash),permissions(permissions),s(OFFLINE){}

Employee Employee::newEmployee(const string& name,double salary,const string& pwd){
  const unsigned char salt[32];
  const unsigned char hash[32];
  const unsigned char* saltedPwd;
  UUID id = UUID::ofNow();
  int outSize = name.length()+32-(name.length()%32);
  if(RAND_status()==0)
    seedSystemRandom();
 RAND_bytes(salt,32);
 saltedPwd = salt(pwd.c_str(),pwd.length(),salt);
 SHA256(saltedPwd,outSize,hash);
 delete[] saltedPwd;
 return Employee(name,id,salary,salt,hash,EnumSet<Permission>(Permission::AUTH));
}

int32_t Employee::hashCode()const{
  int32_t hash = 0;
  const int32_t prime = 31;
  hash += hashcode(name);
  hash *= prime;
  hash += hashcode(id);
  hash *= prime;
  hash += hashcode(salary);
  hash *= prime;
  hash += hashcode(s);
  return hash;
}

const string& Employee::getName()const{
  return name;
}

bool Employee::hasPermission(Permission p)const{
  return permissions.contains(p);
}

void Employee::addPermission(Permission p){
  permissions.add(p);
}

void Employee::removePermission(Permission p){
  permissions.remove(p);
}

void Employee::getSalt(unsigned char(&out)[32])const{
  memcpy(out,this->salt,32);
}

void Employee::getHash(unsigned char(&out)[32])const{
  memcpy(out,this->hash,32);
}

AuthenticationResult Employee::authenticate(const string& s){
  int outSize = name.length()+32-(name.length()%32);
  const char buffer[32];
  if(!permissions.contains(Permission::AUTH))
    return AuthenticationResult::FAIL_CANT_AUTHENTICATE;
  const char* saltedPwd = salt(s.c_str(),s.length(),salt);
  SHA256(saltedPwd,outSize,buffer);
  if(memcmp(hash,buffer,32)!=0)
    return AuthenticationResult::FAIL_BAD_PASSWORD;
  else if(permissions.contains(Permission::ADMIN))
    return AuthenticationResult::SUCCESS_ADMIN;
  else
    return AuthenticationResult::SUCCESS;
}
