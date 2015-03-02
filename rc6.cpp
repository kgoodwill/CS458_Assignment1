#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#define P32 0xB7E15163
#define Q32 0x9E3779B9

using namespace std;
int WORD_SIZE;
int ROUNDS;
int KEY_BYTES;

unsigned long stringToHex(string s);
int plus(int a, int b);
int minus(int a, int b);
int times(int a, int b);
long leftRotate(long num, long amount);
long rightRotate(long num, long amount);
long maskMaker(int size);
int32_t changeEndianness32(int32_t val);
long* changeEndianness(long arr[]);

int main(int argc, char* argv[]){
	if(argc != 6){
		cout << "Error" << endl;
		cout << "Usage:" << endl;
		cout << " ./rc6 W R B <input_file> <output_file>" << endl;
	}
	WORD_SIZE = stoi(argv[1]);
	ROUNDS = stoi(argv[2]);
	KEY_BYTES = stoi(argv[3]);
	ifstream inFile;
	inFile.open(argv[4]);
	string line, type, key, codetext, text1, text2, garbage;
	int i = 0;
	while(getline(inFile, line)){
		stringstream s(line);
		if(i == 0){
			s >> type;
		}
		if(i == 1){
			s >> garbage;
			while(s >> text1){
				codetext += text1;
			}
		}
		if(i == 2){
			s >> garbage;
			while(s >> text2){
				key += text2;
			}
		}
		i++;
	}
	string aString,bString,cString,dString;
	aString = codetext.substr(0,8);
	bString = codetext.substr(8,8);
	cString = codetext.substr(16,8);
	dString = codetext.substr(24,8);

	stringstream buffer(aString);
	unsigned long a_hex, b_hex, c_hex, d_hex;
	buffer >> hex >> a_hex;
	
	d_hex = stringToHex(dString);
	c_hex = stringToHex(cString);
	b_hex = stringToHex(bString);
	a_hex = stringToHex(aString);
	
	inFile.close();

	
	
}

long* keySchedule(string key){
	//KEY_BYTES number of bytes in the key == KEY_BYTES * 8 number of bits
	int words = (KEY_BYTES*8)/32;//c
	long* keyArray = new long[words];
	 
}

long* changeEndiannessArray(long* arr){
	//Swap the endianness of each 32 bit number
	for(int i = 0; i < sizeof(arr)/sizeof(*arr); i++){
		long tmp = changeEndianness32(arr[i]);
		arr[i] = tmp;
	}
	//Swap the endianness of the array itself
}

int32_t changeEndianness32(int32_t val){
	int32_t tmp = (val << 16) | ((val >> 16) & 0x00ffff);
	return ((tmp >> 8) & 0x00ff00ff) | ((tmp & 0x00ff00ff) << 8);
}

unsigned long stringToHex(string s){
	stringstream buffer(s);
	unsigned long hex_num;
	buffer >> hex >> hex_num;
	return hex_num;
}

int plus(int a, int b){
	return (a+b)%(int)(pow(2,WORD_SIZE));
}

int minus(int a, int b){
	return (a-b)%(int)(pow(2,WORD_SIZE));
}

int times(int a, int b){
	return (a*b)%(int)(pow(2,WORD_SIZE));
}

long leftRotate(long num, long amount){
	long numBits = log2(WORD_SIZE);
	long mask1, shiftCountMask, mask3;
	mask1 = maskMaker(WORD_SIZE);
	shiftCountMask = maskMaker(numBits);
	long shiftBits = shiftCountMask & amount;
	mask3 = maskMaker(shiftBits);
	long long x = mask3 << WORD_SIZE;
	long long A = (num << shiftBits) & x;
	long long B = (num << shiftBits) & mask1;
	long long C = A >> WORD_SIZE;
	return B|C;
}

long rightRotate(long num, long amount){
	long shiftCountMask, rightMask;
	long numBits = log2(WORD_SIZE);
	shiftCountMask = maskMaker(numBits);
	long shiftBits = shiftCountMask & amount;
	rightMask = maskMaker(shiftBits);
	rightMask = rightMask & num;//Now contains the right portion of the number
	long shiftedNum = num >> shiftBits;
	long leftPortion = rightMask << (WORD_SIZE - shiftBits);
	return shiftedNum | leftPortion;
}

long maskMaker(int size){
	long mask = 0b0;
	for(int i = 0; i < size; i++){
		mask <<= 1;
		mask++;
	}
	return mask;			
}
