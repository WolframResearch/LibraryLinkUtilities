/**
 * @file	MArgumentManagerTest.cpp
 * @author	rafalc
 * @brief	Source code for unit tests of MArgumentManager
 */
#include <functional>
#include <tuple>

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return LLU::ErrorCode::NoError;
}

// Simple case for single-type custom MArgument types - directly specialize MArgumentManager::get.
namespace LLU {
	template<>
	float MArgumentManager::get<float>(size_type index) const {
		return static_cast<float>(get<double>(index));
	}
}

LLU_LIBRARY_FUNCTION(AsFloat) {
	auto [f1] = mngr.getTuple<float>();
	auto f2 = mngr.get<float>(0);
	if (f1 != f2) {
		LLU::ErrorManager::throwException(LLU::ErrorName::FunctionError);
	}
	mngr.set(static_cast<double>(f1));
}

LLU_LIBRARY_FUNCTION(DescribePerson) {
	auto [name, age, height] = mngr.getTuple<std::string, uint8_t, double>();
	mngr.set(name + " is " + std::to_string(age) + " years old and " + std::to_string(height) + "m tall.");
}

struct Person {
	Person(std::string n, uint8_t a, double h) : name {std::move(n)}, age {a}, height {h} {}

	std::string name;
	uint8_t age;
	double height;

	[[nodiscard]] std::string description() const {
		return name + " is " + std::to_string(age) + " years old and " + std::to_string(height) + "m tall.";
	}
};

namespace LLU {
	template<>
	struct MArgumentManager::CustomType<Person> { using CorrespondingTypes = std::tuple<std::string, uint8_t, double>; };

	template<>
	void MArgumentManager::set<Person>(const Person& p) {
		DataList<MArgumentType::MArgument> personDS;
		personDS.push_back<MArgumentType::UTF8String>(const_cast<char*>(p.name.c_str()));
		personDS.push_back<MArgumentType::Integer>(p.age);
		personDS.push_back<MArgumentType::Real>(p.height);
		set(personDS);
	}
}

LLU_LIBRARY_FUNCTION(DescribePerson2) {
	auto person = mngr.get<Person>(0);
	mngr.set(person.description());
}

LLU_LIBRARY_FUNCTION(ComparePeople) {
	using namespace std::string_literals;
	auto personA = mngr.get<Person>(0);
	auto personB = mngr.get<Person>(3);
	mngr.set(personA.name + " is" + (personA.height > personB.height? ""s : " not"s) + " taller than " + personB.name + ".");
}

LLU_LIBRARY_FUNCTION(PredictChild) {
	auto [personA, personB] = mngr.getTuple<Person, Person>();
	mngr.set(Person {personA.name + " Junior", 0, (personA.height + personB.height) / 2});
}

namespace LLU {
	template<typename T>
	struct MArgumentManager::CustomType<std::vector<T>> { using CorrespondingTypes = std::tuple<NumericArray<T>>; };

	template<typename T>
	struct MArgumentManager::MArgGetter<std::vector<T>> {
		static std::vector<T> get(const MArgumentManager& mngr, size_type index) {
			auto na = mngr.get<NumericArray<T>>(index);
			return { std::cbegin(na), std::cend(na) };
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Experimental functionality for automatic generation of library functions from regular functions.
/// Not mature enough to be included in LLU, but paclets may copy and use this code (with caution).
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename R, typename... Args>
void runAsLibraryFunction(LLU::MArgumentManager& mngr, std::function<R(Args...)> f) {
	mngr.set(std::apply(std::move(f), mngr.getTuple<Args...>()));
}

template<typename R, typename... Args>
void runAsLibraryFunction(LLU::MArgumentManager& mngr, R(*f)(Args...)) {
	if constexpr (std::is_same_v<R, void>) {
		std::apply(f, mngr.getTuple<std::decay_t<Args>...>());
	} else {
		mngr.set(std::apply(f, mngr.getTuple<std::decay_t<Args>...>()));
	}
}

template<class C, typename R, typename... Args>
void runAsLibraryFunction(LLU::MArgumentManager& mngr, R(C::*f)(Args...) const) {
	if constexpr (std::is_same_v<R, void>) {
		std::apply(f, mngr.getTuple<C, std::decay_t<Args>...>());
	} else {
		mngr.set(std::apply(f, mngr.getTuple<C, std::decay_t<Args>...>()));
	}
}

#define LIBRARIFY(function) LIBRARIFY_TO(function, LL_##function)

#define LIBRARIFY_TO(function, topLevelName)      \
	LLU_LIBRARY_FUNCTION(topLevelName) {          \
		try {                                     \
			runAsLibraryFunction(mngr, function); \
		} catch (const std::exception& e) {        \
			std::cout << e.what() << std::endl;   \
		}                                         \
	}

std::string repeatString(const std::string& s, unsigned int n) {
	std::string res;
	while (n --> 0) {
		res += s;
	}
	return res;
}
LIBRARIFY(repeatString)

void doNothing() noexcept {}
LIBRARIFY(doNothing)

LIBRARIFY_TO(&Person::description, GetPersonDescription)