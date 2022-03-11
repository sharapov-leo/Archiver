#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <experimental/filesystem>
#include <iterator>
#include "algorithms.h"

void readStr(std::istream* inputStr, std::vector<unsigned char>& S, int& blockSize) {
	int actBlockSize = 0;
	for (;;) {
		int c = inputStr->get();
		if (c == EOF)
			break;
		S.push_back(c);
		++actBlockSize;
		if (actBlockSize == blockSize)
			return;
	}
	blockSize = actBlockSize;
}

void writeStr(std::ostream* outputStr, std::vector<unsigned char>& S) {
	for(auto s : S)
		outputStr->put(s);
}

void encode(std::istream* inputStr, std::ostream* outputStr, std::vector<unsigned char>& S, std::vector<unsigned char>& L, int& blockSize, int& I, std::list<unsigned char>& X, std::vector<int>& R, std::vector<int>& E) {
	readStr(inputStr, S, blockSize);
	algorithmC(S, L, I);
	algorithmM1(L, X, R);
	RLEncode(R, E);	
}

void decode(std::ostream* outputStr, std::vector<int>& E, std::list<unsigned char>& X, int& I, std::vector<int>& R, std::vector<unsigned char>& L, std::vector<unsigned char>& S) {
	RLDecode(E, R);
	algorithmW2(X, R, L);
	algorithmD(L, I, S);
	writeStr(outputStr, S);
}

void deleteSource(std::string fileName) {
	std::remove(fileName.c_str());	
}

void archiver(int argc, char* argv[]) {
	std::vector<unsigned char> S;
	std::vector<unsigned char> L;

	int blockSize;
	int I;

	std::vector<int> R;
	std::vector<int> E;
	std::list<unsigned char> X;

	bool isStdout			= false;
	bool isCompress			= true;
	bool isDecompress		= false;
	bool isDeleteSource		= true;
	bool isProcDirectory	= false;
	bool isBestCompress		= true;

	std::istream* inputStr	= nullptr;
	std::ostream* outputStr = nullptr;
	std::ifstream fIn;
	std::ofstream fOut;

	std::string fileName;
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-c"))
			isStdout = true;
		else if (!strcmp(argv[i], "-d"))
			isDecompress = true;
		else if (!strcmp(argv[i], "-k"))
			isDeleteSource = false;
		else if (!strcmp(argv[i], "-r"))
			isProcDirectory = true;
		else if (!strcmp(argv[i], "-1")) {
			isBestCompress = false;
			blockSize = 102400;
		}
		else if (!strcmp(argv[i], "-9"))
			blockSize = 921600;
		else
			fileName = argv[i];
	}
	if (isStdout)
		isDeleteSource = false;
	if (isDecompress) {
		isCompress = false;
	}

	std::vector<std::string> fileNames;
	if (std::experimental::filesystem::is_directory(fileName)) {
		if (!isProcDirectory) {
			std::cout << fileName << " is a directory -- ignored" << std::endl;
			exit(-3);
		}
		std::experimental::filesystem::recursive_directory_iterator elem(fileName), end;
		while (elem != end) {
			if (std::experimental::filesystem::is_regular_file(elem->path()))
				fileNames.push_back(elem->path().string());
			++elem;
		}
	}
	else
		fileNames.push_back(fileName);

	for (auto fileToComp : fileNames) {
		fIn.open(fileToComp, std::ios::in | std::ios::binary);
		if (!fIn.is_open()) {
			std::cout << fileToComp << ": file open error" << std::endl;
			continue;
		}
		inputStr = &fIn;

		if (isStdout)
			outputStr = &std::cout;
		else {
			if (isCompress) {
				if ((fileToComp.size() >= 6) && ((fileToComp.substr(fileToComp.size() - 6, 6)) == ".sharp")) {
					std::cout << fileToComp << ": has already .sharp suffix -- unchanged" << std::endl;
					continue;
				}
				fOut.open(fileToComp + ".sharp", std::ios::out | std::ios::binary);
				if (!fOut.is_open()) {
					std::cout << fileToComp << ".sharp: file write error" << std::endl;
					continue;
				}
				outputStr = &fOut;
			}
			if (isDecompress) {
				if (fileToComp.size() >= 6) {
					if (fileToComp.substr(fileToComp.size() - 6, 6) != ".sharp") {
						std::cout << fileToComp << ": unknown suffix -- ignored" << std::endl;
						continue;
					}
				}
				else {
					std::cout << fileToComp << ": unknown suffix -- ignored" << std::endl;
					continue;
				}
				fOut.open(fileToComp.substr(0, fileToComp.size() - 6), std::ios::out | std::ios::binary);
				if (!fOut.is_open()) {
					std::cout << fileToComp.substr(0, fileToComp.size() - 6) << ": file write error" << std::endl;
					continue;
				}
				outputStr = &fOut;
			}
		}
		if (isCompress) {
			uintmax_t fileSize = std::experimental::filesystem::file_size(fileToComp);
			for (uintmax_t curSize = 0; curSize < fileSize; curSize += blockSize) {
				encode(inputStr, outputStr, S, L, blockSize, I, X, R, E);
				outputStr->put((char)(I & 255));
				outputStr->put((char)((I >> 8) & 255));
				outputStr->put((char)((I >> 16) & 255));
				outputStr->put((char)((I >> 24) & 255));

				char NX = (char)X.size();
				outputStr->put(NX);

				for (auto x : X)
					outputStr->put(x);

				int NE = (int)E.size();
				outputStr->put((char)(NE & 255));
				outputStr->put((char)((NE >> 8) & 255));
				outputStr->put((char)((NE >> 16) & 255));
				outputStr->put((char)((NE >> 24) & 255));

				for (auto e : E)
					outputStr->put((char)(e & 255));
				S.clear();
				L.clear();
				X.clear();
				R.clear();
				E.clear();
			}

		}
		if (isDecompress) {
			char c;
			for (;;) {
				c = inputStr->get();
				if (c == EOF)
					break;
				I = (unsigned char)c;
				c = inputStr->get();
				I += ((unsigned char)c << 8);
				c = inputStr->get();
				I += ((unsigned char)c << 16);
				c = inputStr->get();
				I += ((unsigned char)c << 24);

				c = inputStr->get();
				unsigned char NX = (unsigned char)c;

				for (int i = 0; i < NX; ++i) {
					c = inputStr->get();
					X.push_back((unsigned char)c);
				}

				c = inputStr->get();
				int NE = (unsigned char)c;
				c = inputStr->get();
				NE += ((unsigned char)c << 8);
				c = inputStr->get();
				NE += ((unsigned char)c << 16);
				c = inputStr->get();
				NE += ((unsigned char)c << 24);

				for (int i = 0; i < NE; ++i) {
					c = inputStr->get();
					E.push_back((unsigned char)c);
				}
				decode(outputStr, E, X, I, R, L, S);
				S.clear();
				L.clear();
				X.clear();
				R.clear();
				E.clear();
			}
		}
		fIn.close();
		if (fOut.is_open())
			fOut.close();
		if (isDeleteSource)
			deleteSource(fileToComp);
	}

}


int main(int argc, char* argv[]) {
	archiver(argc, argv);

	return 0;
}
