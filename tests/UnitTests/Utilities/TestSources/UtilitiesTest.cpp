/**
 * @file	UtilitiesTest.cpp
 * @brief
 */

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>


EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return 0;
}

mint openFile(const std::string& path, std::ios::openmode mode) {
	auto filePtr = LLU::openFile(path, mode);
	return std::ftell(filePtr.get());
}

LLU_LIBRARY_FUNCTION(OpenForReading) {
	auto filePath = mngr.getString(0);
	mngr.setInteger(openFile(filePath, std::ios::in));
}

LLU_LIBRARY_FUNCTION(OpenForWriting) {
	auto filePath = mngr.getString(0);
	mngr.setInteger(openFile(filePath, std::ios::out));
}

LLU_LIBRARY_FUNCTION(OpenInvalidMode) {
	auto filePath = mngr.getString(0);
	mngr.setInteger(openFile(filePath, std::ios::in|std::ios::trunc));
}

LLU_LIBRARY_FUNCTION(WriteStrings) {
	auto filePath = mngr.getString(0);
	auto wordList = mngr.getDataList<LLU::MArgumentType::UTF8String>(1);
	auto f = LLU::openFileStream(filePath, std::ios::out|std::ios::trunc);
	for (auto&& word : wordList) {
		f << word.getValue() << "\n";
	}
}

LLU_LIBRARY_FUNCTION(ReadStrings) {
	auto filePath = mngr.getString(0);
	auto f = LLU::openFileStream(filePath, std::ios::in);
	LLU::DataList<LLU::MArgumentType::UTF8String> wordList;
	std::string word;
	while (f >> word) {
		wordList.push_back(const_cast<char*>(word.c_str()));
	}
	mngr.set(wordList);
}