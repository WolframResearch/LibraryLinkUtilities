/**
 * @file	UtilitiesTest.cpp
 * @brief
 */

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

class File {
public:
	File(const std::string& path, std::ios::openmode mode) {
		f = LLU::openFile(path, mode);
	}
private:
	LLU::FilePtr f {nullptr, [](FILE*) { return 0; }};
};

DEFINE_MANAGED_STORE_AND_SPECIALIZATION(File)

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	FileStore.registerType("MyFile");
	return 0;
}

mint openFile(const std::string& path, std::ios::openmode mode) {
	auto filePtr = LLU::openFile(path, mode);
	return std::ftell(filePtr.get());
}

LIBRARY_LINK_FUNCTION(OpenManagedFile) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);
		auto fname = mngr.getString(1);
		auto modeInt = mngr.getInteger<mint>(2);
		std::ios::openmode mode = modeInt == 0? std::ios::in : (modeInt == 1? std::ios::out : std::ios::in|std::ios::out);
		FileStore.createInstance(id, fname, mode);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
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