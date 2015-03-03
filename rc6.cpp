#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>

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
unsigned long leftRotate(unsigned long num, unsigned long amount);
unsigned long rightRotate(unsigned long num, unsigned long amount);
long maskMaker(int size);
int32_t changeEndianness32(int32_t val);
vector<long> changeEndiannessVector(vector<long> vec);
vector<long> keySchedule(string key);
vector<long> encrypt(vector<long> input, vector<long> keys);
vector<long> decrypt(vector<long> input, vector<long> keys);

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
	
	vector<long> inputText;
	inputText.push_back(a_hex);
	inputText.push_back(b_hex);
	inputText.push_back(c_hex);
	inputText.push_back(d_hex);

	inFile.close();

	vector<long> keys = keySchedule(key);
	
	/*for(int i = 0; i < keys.size(); i++){
		cout << hex << keys[i] << endl;
	}
	cout << int << keys.size() << endl;*/

	cout << "Original" << endl;
	for(int i = 0; i < inputText.size(); i++){
		cout << hex << inputText[i] << endl;
	}
	cout << "Encrypted" << endl;
	vector<long> ciphertext = encrypt(inputText, keys);
	for(int i = 0; i < ciphertext.size(); i++){
		cout << hex << ciphertext[i] << endl;
	}
	cout << "Decrypted" << endl;
	vector<long> decrypttext = decrypt(ciphertext, keys);
	for(int i = 0; i < decrypttext.size(); i++){
		cout << hex << decrypttext[i] << endl;
	}
}


vector<long> keySchedule(string key){
	//KEY_BYTES number of bytes in the key == KEY_BYTES * 8 number of bits
	int words = (KEY_BYTES*8)/32;//c
	vector<long> keyVector;
	int index = 0;
	for(int i = 0; i < words; i++){
		//Break the key into "words" number of words
		string tmp = key.substr(index,8);
		keyVector.push_back(stringToHex(tmp));
		index+=8;
	//	cout << hex << stringToHex(tmp) << endl;
	}
	vector<long> roundKeys;
	roundKeys.push_back(P32);
	for(int i = 1; i < (2*ROUNDS + 3); i++){
		roundKeys.push_back((long)roundKeys[i-1] + Q32);
	}

	long A, B, i, j;
	A = B = i = j = 0;
	
	long v = times(3, max(words ,(2*ROUNDS + 3)));
	for(int s = 0; s < v; s++){
		A = roundKeys[i] = leftRotate(roundKeys[i] + A + B, 3);
		B = keyVector[j] = leftRotate(keyVector[j] + A + B, (A+B));
		i = (i+1)%(2*ROUNDS+4);
		j = (j+1)%words;
	}
	return roundKeys;
	 
}

vector<long> encrypt(vector<long> input, vector<long> keys){
	//long A = input[0];
	//long B = input[1];
	//long C = input[2];
	//long D = input[3];

	input[1] = input[1] + keys[0];
	input[3] = input[3] + keys[1];
	long t, u;
	for(int i = 1; i < ROUNDS; i++){
		t = leftRotate((input[1] * (2*input[1] + 1)), log2(WORD_SIZE));
		u = leftRotate((input[3] * (2*input[3] + 1)), log2(WORD_SIZE));
		input[0] = leftRotate((input[0] ^ t), u) + keys[2*i];
		input[2] = leftRotate((input[2] ^ u), t) + keys[(2*i)+1];
		long tmpA, tmpB, tmpC, tmpD;
		tmpA = input[0];
		tmpB = input[1];
		tmpC = input[2];
		tmpD = input[3];
		input[0] = tmpB;
		input[1] = tmpC;
		input[2] = tmpD;
		input[3] = tmpA;
	}
	input[0] = input[0] + keys[2*ROUNDS + 3];
	input[2] = input[2] + keys[2*ROUNDS + 3];
	return input;
}

vector<long> decrypt(vector<long> input, vector<long> keys){
	input[2] = input[2] - keys[2*ROUNDS + 3];
	input[0] = input[0] - keys[2*ROUNDS + 3];
	long t, u;
	for(int i = ROUNDS; i > 1; i--){
		long tmpA, tmpB, tmpC, tmpD;
		tmpA = input[0];
		tmpB = input[1];
		tmpC = input[2];
		tmpD = input[3];
		input[0] = tmpD;
		input[1] = tmpA;
		input[2] = tmpB;
		input[3] = tmpC;
		u = leftRotate(input[3] * (2 * input[3] + 1), log2(WORD_SIZE));
		t = leftRotate(input[1] * (2 * input[1] + 1), log2(WORD_SIZE));
		input[2] = rightRotate(input[2] - keys[2*i + 1], t) ^ u;
		input[0] = rightRotate(input[0] - keys[2*i], u) ^ t;		
	}
	input[3] = input[3] - keys[1];
	input[0] = input[0] - keys[0];
	return input;
}

vector<long> changeEndiannessVector(vector<long> vec){
	//Swap the endianness of each 32 bit number
	for(int i = 0; i < vec.size(); i++){
		long tmp = changeEndianness32(vec[i]);
		vec[i] = tmp;
	}
	//Swap the endianness of the array itself
	for(int i = 0; i < (vec.size())/2; i++){
		long tmp = vec[i];
		vec[i] = vec[(vec.size()-1)-i];
		vec[(vec.size()-1)-i] = tmp;
	}
	return vec;
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

unsigned long leftRotate(unsigned long num, unsigned long amount){
	unsigned long numBits = log2(WORD_SIZE);
	unsigned long mask1, shiftCountMask, mask3;
	mask1 = maskMaker(WORD_SIZE);
	shiftCountMask = maskMaker(numBits);
	unsigned long shiftBits = shiftCountMask & amount;
	mask3 = maskMaker(shiftBits);
	unsigned long long x = mask3 << WORD_SIZE;
	unsigned long long A = (num << shiftBits) & x;
	unsigned long long B = (num << shiftBits) & mask1;
	unsigned long long C = A >> WORD_SIZE;
	return B|C;
}

unsigned long rightRotate(unsigned long num, unsigned long amount){
	unsigned long shiftCountMask, rightMask;
	unsigned long numBits = log2(WORD_SIZE);
	shiftCountMask = maskMaker(numBits);
	unsigned long shiftBits = shiftCountMask & amount;
	rightMask = maskMaker(shiftBits);
	rightMask = rightMask & num;//Now contains the right portion of the number
	unsigned long shiftedNum = num >> shiftBits;
	unsigned long leftPortion = rightMask << (WORD_SIZE - shiftBits);
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
