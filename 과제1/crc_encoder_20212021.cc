#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

string codeword;
vector<string> dataWords;
int dataWordSize;
int generator;
vector<unsigned char> bitwise_EncodedMessage;

ifstream inputFile;
ofstream outputFile;

// CRC 인코딩 함수
void performCRC(string div, int length) {
    int count = 0;
    while (count < dataWords.size()) {
        string remain = dataWords[count];
        string temp = remain;
        for (int j = 0; j < length; j++) {
            remain += "0";
        }
 
        for (int x = 0; x < remain.size() - length; x++) {
            if (remain[x] == '0') {
                continue;
            }
            for (int y = 0; y < div.size(); y++) {
                if (div[y] == '1') {
                    remain[x + y] = (remain[x + y] == '1') ? '0' : '1';
                }
            }
        }
        remain = remain.substr(remain.size() - length);
        temp += remain;
        codeword += temp;
        count++;
    }
}

// 입력 파일에서 데이터를 읽는 함수
void readInput(int dataSize, int length) {
    while (inputFile.peek() != EOF) {
        char x;
        inputFile.get(x);
        if (dataSize == 4) {
            bitset<8> bitset(x);
            string bitString = bitset.to_string();
            dataWords.push_back(bitString.substr(0, 4));
            dataWords.push_back(bitString.substr(4, 4));
        }
        else if (dataSize == 8) {
            bitset<8> bitset(x);
            dataWords.push_back(bitset.to_string());
        }
    }
}


// 문자열을 바이트 단위로 변환하는 함수
void stringToByte() {
    while (!codeword.empty()) {
        string bits_8 = codeword.substr(0, 8);
        codeword.erase(0, 8);
        unsigned char byte = 0;
        for (int x = 0; x < 8; x++) {
            byte |= (bits_8[x] - '0') << (7 - x);
        }
        bitwise_EncodedMessage.push_back(byte);
    }
}

// 출력 파일에 결과를 쓰는 함수
void writeOutput() {
    int padding_Num = codeword.size() % 8;
    padding_Num = (padding_Num == 0) ? 0 : (8 - padding_Num);
    bitset<8> padding(padding_Num);
    string paddingStr = padding.to_string();

    if (padding_Num) {

        for (int x = 0; x < padding_Num; x++) {
            codeword = "0" + codeword;
        }
    }
    codeword = paddingStr + codeword;

    stringToByte();
    for (int y = 0; y < bitwise_EncodedMessage.size(); y++) {
        outputFile.put(bitwise_EncodedMessage[y]);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 5) {
        cout << "Usage: ./crc_encoder input_file output_file generator_polynomial dataword_size\n";
        return 1;
    }

    inputFile.open(argv[1]);
    if (!inputFile) {
        cout << "input file open error.\n";
        return 1;
    }
    outputFile.open(argv[2], ios::binary);
    if (!outputFile) {
        cout << "output file open error.\n";
        return 1;
    }

    dataWordSize = argv[4][0] - '0';
    if (dataWordSize != 4 && dataWordSize != 8) {
        cout << "dataword size must be 4 or 8.\n";
        return 1;
    }

    string polynomialStr = argv[3];
    for (int x = 1; x <= polynomialStr.size(); x++) {
        if (polynomialStr[polynomialStr.size() - x] == '1') {
            unsigned int temp = 1;
            for (int y = 0; y < x - 1; y++)
            {
                temp = temp << 1;
            }
            generator += temp;
        }
    }
    readInput(dataWordSize, polynomialStr.size());
    performCRC(polynomialStr, polynomialStr.size() - 1);
    writeOutput();

    return 0;
}
