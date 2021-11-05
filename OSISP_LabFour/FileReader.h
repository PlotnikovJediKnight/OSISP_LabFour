#pragma once
#include <vector>
#include <fstream>
#include <string>

class FileReader {
public:
	static FileReader& Instance();

	using Strings = std::vector<std::string>;
	void SetFilePath(char* lp_file_path);
	const char* GetFilePath();
	void DoReadFile();
	Strings GetFileStrings();


private:
	FileReader() = default;
	~FileReader() = default;
	FileReader(const FileReader&) = delete;
	FileReader& operator=(const FileReader&) = delete;

	char* _lp_file_path;
	Strings _file_strings;
};

