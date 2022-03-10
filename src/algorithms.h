#include <string>
#include <memory>
#include <vector>
#include <list>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <list>

const int alphabetSize = 256;
const int UCHARMAX = 255;

// BWT
void algorithmC(std::vector<unsigned char>& S, std::vector<unsigned char>& L, int& I);
void algorithmD(std::vector<unsigned char>& L, int& I, std::vector<unsigned char>& S);

// MTF
void algorithmM1(std::vector<unsigned char>& L, std::list<unsigned char>& X, std::vector<int>& R);
void algorithmW2(std::list<unsigned char>& X, std::vector<int>& R, std::vector<unsigned char>& L);

// RLE
void RLEncode(std::vector<int>& R, std::vector<int>& E);
void RLDecode(std::vector<int>& E, std::vector<int>& R);