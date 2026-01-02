#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;

int datawordSize;
int codewordSize;
int errorCount = 0;
int codewordCount = 0;

ifstream inputFile;
ofstream outputFile;
ofstream resultFile;

string codeword;
string generator;


void readPadding() {//코드워드에서 처음 8 bit를 padding으로 사용하고, 해당 부분을 제거하는 함수
    string padding = codeword.substr(0, 8);
    codeword.erase(0, 8);
    int paddingSize = stoi(padding, nullptr, 2);
    codeword.erase(0, paddingSize);
}

void readInput() { //입력 파일을 이진 모드로 열고, 파일의 크기를 계산해 해당 size만큼의 bit를 읽어와 codeword에 저장
    inputFile.seekg(0, inputFile.end);
    int size = inputFile.tellg();
    inputFile.seekg(0, inputFile.beg);

    codeword.reserve(size * 8); 
    for (int x = 0; x < size; x++) {
        char byte;
        inputFile.get(byte);
        codeword += bitset<8>(byte).to_string();
    }
}


string modDivision(string remain, string divisor, int length) {
    //CRC 연산을 수행하는 함수로
    for (int x = 0; x < remain.size() - length; x++) {
        if (remain[x] == '0') continue;
        for (int y = 0; y < divisor.size(); y++) {
            if (divisor[y] == '1') {
                remain[x + y] = (remain[x + y] == '1') ? '0' : '1';
            }
        }
    }
    remain.erase(0, remain.size() - length);
    return remain;
}

void redundancyCheck() {
    //각 코드워드에 대해 CRC 중복성 검사를 수행하여 오류를 찾고, 발견된 오류 수와 전체 코드워드 수를 계산
    while (!codeword.empty()) {
        string temp = codeword.substr(0, codewordSize);
        codeword.erase(0, codewordSize);
        string syndrome = modDivision(temp, generator, generator.size() - 1);
        for (char ch : syndrome) {
            if (ch != '0') {
                errorCount++;
                break;
            }
        }
        codewordCount++;
    }
}

void decodeAndWrite() {
    //디코딩된 코드워드를 출력 파일에 쓰는 함수
    while (!codeword.empty()) {
        string temp = codeword.substr(0, 8);
        codeword.erase(0, 8);
        bitset<8> bits(temp);
        outputFile.put(bits.to_ulong());
    }
    resultFile << codewordCount << " " << errorCount;
}

int main(int argc, char* argv[]) {

    if (argc != 6) {
        cout << "Usage: ./crc_decoder input_file output_file result_file generator_polynomial dataword_size\n";
        return 1;
    }

    inputFile.open(argv[1], ios::binary);
    if (!inputFile) {
        cout << "input file open error.\n";
        return 1;
    }

    outputFile.open(argv[2], ios::binary);
    if (!outputFile) {
        cout << "output file open error.\n";
        return 1;
    }

    resultFile.open(argv[3]);
    if (!resultFile) {
        cout << "result file open error.\n";
        return 1;
    }

    datawordSize = argv[5][0] - '0';
    if (datawordSize != 4 && datawordSize != 8) {
        cout << "dataword size must be 4 or 8.\n";
        return 1;
    }

    generator = argv[4];
    codewordSize = datawordSize + generator.size() - 1;

    readInput();
    readPadding();
    redundancyCheck();
    decodeAndWrite();

    return 0;
}

