/**
 * @file	MArgumentManagerTest.cpp
 * @author	rafalc
 * @brief	Source code for unit tests of MArgumentManager
 */
#include <functional>
#include <tuple>

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

/* Initialize Library */

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return LLU::ErrorCode::NoError;
}


LLU_LIBRARY_FUNCTION(DescribePerson) {
	auto [name, age, height] = mngr.get<std::string, uint8_t, double>();
	mngr.set(name + " is " + std::to_string(age) + " years old and " + std::to_string(height) + "m tall.");
}

struct Person {
	Person(std::string n, uint8_t a, double h) : name {std::move(n)}, age {a}, height {h} {}

	std::string name;
	uint8_t age;
	double height;

	std::string description() const {
		return name + " is " + std::to_string(age) + " years old and " + std::to_string(height) + "m tall.";
	}
};

namespace LLU {
	template<>
	Person MArgumentManager::get<Person>(size_type index) const {
		return std::make_from_tuple<Person>(get<std::string, uint8_t, double>({index, index + 1, index + 2}));
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

template<typename R, typename... Args>
void runAsLibraryFunction(LLU::MArgumentManager& mngr, std::function<R(Args...)> f) {
	mngr.set(std::apply(std::move(f), mngr.get<Args...>()));
}

template<typename R, typename... Args>
void runAsLibraryFunction(LLU::MArgumentManager& mngr, R(*f)(Args...)) {
	if constexpr (std::is_same_v<R, void>) {
		std::apply(f, mngr.get<std::decay_t<Args>...>());
	} else {
		mngr.set(std::apply(f, mngr.get<std::decay_t<Args>...>()));
	}
}

template<class C, typename R, typename... Args>
void runAsLibraryFunction(LLU::MArgumentManager& mngr, R(C::*f)(Args...) const) {
	if constexpr (std::is_same_v<R, void>) {
		std::apply(f, mngr.get<C, std::decay_t<Args>...>());
	} else {
		mngr.set(std::apply(f, mngr.get<C, std::decay_t<Args>...>()));
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