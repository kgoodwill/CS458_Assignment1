#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

//#define WORD_SIZE 32
//#define ROUNDS 20

using namespace std;

int main(int argc, char* argv[]){
	if(argc != 6){
		cout << "Error" << endl;
		cout << "Usage:" << endl;
		cout << " ./rc6 W R B <input_file> <output_file>" << endl;
	}
	#define WORD_SIZE argv[2]
	#define ROUNDS argv[3]
	#define KEY_BYTES argv[4]
	ifstream inFile;
	inFile.open(argv[4]);
	//TODO Read the input as hex and break into ABCD registers
	string type, numbers, key;
	getline(inFile,type);
	getline(inFile,numbers);
	getline(inFile,key);
	cout << type << endl;
	cout << numbers << endl;
	cout << key << endl;
	inFile.close();
}
