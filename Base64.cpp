#include <string>
#include "Base64.hpp"

#define B64OffSetCharacter '+'
#define B64PaddingCharacter '='

using std::string;

char b64EncodeMap[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
					   'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
					   'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
					   'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'};
int b64DecodeMap[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,
					  -1,-1,-1,-1,-1,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					  10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
					  -1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,
					  36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};

string encodeBase64(unsigned const char* data,int len){
	int word;
	string ret;
	for(int i = 0;i<len/3;i++){
		word = data[i]|data[i+1]<<8|data[i+2]<<16;
		for(int j = 0;j<4;j++){
			ret += b64EncodeMap[word%64];
			word /= 64;
		}
	}
	for(int i = 0;i<ret.length()%4;i++)
		ret += B64PaddingCharacter;
	return ret;
}

void decodeBase64(unsigned char* data,string base64){
	int tmpWord;
	for(int i = 0;i<base64.length()/4;i++){
		tmpWord = 0;
		for(int j = 0;j<4;j++){
			if(base64[i*4+j]==B64PaddingCharacter)
				return;
			tmpWord *= 64;
			tmpWord += b64DecodeMap[base64[i*4+j]-B64OffSetCharacter];
		}
		data[i*3] = (tmpWord)&0xFF;
		data[i*3+1] = (tmpWord>>8)&0xFF;
		data[i*3+2] = (tmpWord>>16)&0xFF;
	}
}