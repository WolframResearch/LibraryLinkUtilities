/**
 * @file	UtilitiesTest.cpp
 * @brief
 */

#include <fstream>

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

class File {
public:
	File(const std::string& path, std::ios::openmode mode) {
		f = LLU::openFile(path, mode); // open with default Import/Export policy
	}
private:
	LLU::FilePtr f {nullptr, [](FILE*) { return 0; }};
};

class FileStream {
public:
	FileStream(const std::string& path, std::ios::openmode mode) {
		f = LLU::openFileStream(path, mode, LLU::SharePolicy {}); // open with _SH_SECURE
	}
private:
	std::fstream f;
};

DEFINE_MANAGED_STORE_AND_SPECIALIZATION(File)
DEFINE_MANAGED_STORE_AND_SPECIALIZATION(FileStream)

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	FileStore.registerType("MyFile");
	FileStreamStore.registerType("FileStream");
	return 0;
}

mint openFile(const std::string& path, std::ios::openmode mode) {
	auto filePtr = LLU::openFile(path, mode);
	return std::ftell(filePtr.get());
}

LLU_LIBRARY_FUNCTION(OpenManagedFile) {
	auto id = mngr.getInteger<mint>(0);
	auto fname = mngr.getString(1);
	auto modeInt = mngr.getInteger<mint>(2);
	std::ios::openmode mode = modeInt == 0? std::ios::in : (modeInt == 1? std::ios::out : std::ios::in|std::ios::out);
	FileStore.createInstance(id, fname, mode);
}

LLU_LIBRARY_FUNCTION(OpenManagedFileStream) {
	auto id = mngr.getInteger<mint>(0);
	auto fname = mngr.getString(1);
	auto modeInt = mngr.getInteger<mint>(2);
	std::ios::openmode mode = modeInt == 0? std::ios::in : (modeInt == 1? std::ios::out : std::ios::in|std::ios::out);
	FileStreamStore.createInstance(id, fname, mode);
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

static std::wstring toWideStr(const std::u16string& u16) {
	return {u16.begin(), u16.end()};
}

LLU_LIBRARY_FUNCTION(WideStringUTF8UTF16Conversion) {
	std::string u8 = u8"z\u00df\u6c34\U0001f34c";
	std::wstring u16wide = toWideStr(u"z\u00df\u6c34\U0001f34c");

	auto u16to8 = LLU::fromUTF16toUTF8(u16wide);
	bool isU16ToU8ok = (u8 == u16to8);

	auto u8to16 = LLU::fromUTF8toUTF16<wchar_t>(u8);
	bool isU8ToU16ok = (u16wide == u8to16);

	mngr.set(isU8ToU16ok && isU16ToU8ok);
}

LLU_LIBRARY_FUNCTION(Char16UTF8UTF16Conversion) {
	std::string u8 = u8"z\u00df\u6c34\U0001f34c";
	std::u16string u16 = u"z\u00df\u6c34\U0001f34c";

	auto u16to8 = LLU::fromUTF16toUTF8(u16);
	bool isU16ToU8ok = (u8 == u16to8);

	auto u8to16 = LLU::fromUTF8toUTF16<char16_t>(u8);
	bool isU8ToU16ok = (u16 == u8to16);

	mngr.set(isU8ToU16ok && isU16ToU8ok);
}

LLU_LIBRARY_FUNCTION(UTF8ToUTF16Bytes) {
	auto u8str = mngr.getString(0);
	std::u16string u16str = LLU::fromUTF8toUTF16<char16_t>(u8str);
	LLU::NumericArray<std::uint16_t> u16bytes {reinterpret_cast<const std::uint16_t*>(u16str.data()), reinterpret_cast<const std::uint16_t*>(u16str.data() + u16str.length())};
	mngr.set(u16bytes);
}

LLU_LIBRARY_FUNCTION(UTF16BytesToUTF8) {
	auto u16bytes = mngr.getNumericArray<uint16_t>(0);
	std::wstring u16wideStr;
	std::transform(u16bytes.cbegin(), u16bytes.cend(), std::back_inserter(u16wideStr), [](uint16_t cp) {
		return static_cast<wchar_t>(cp);
	});
	std::string u8str = LLU::fromUTF16toUTF8<wchar_t>(u16wideStr);
	mngr.set(u8str);
}