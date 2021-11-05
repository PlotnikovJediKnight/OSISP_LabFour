#include "FileReader.h"
using namespace std;

FileReader& FileReader::Instance() {
	static FileReader instance;
	return instance;
}

void FileReader::SetFilePath(char* lp_file_path) { this->_lp_file_path = lp_file_path; }

const char* FileReader::GetFilePath() { return _lp_file_path; }

void FileReader::DoReadFile() {
	ifstream in(_lp_file_path);
	_file_strings.clear();

	string s;
	while (in >> s) {
		_file_strings.push_back(move(s));
	}

	in.close();
}

FileReader::Strings FileReader::GetFileStrings() {
	return _file_strings;
}