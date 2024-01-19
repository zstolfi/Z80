#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <queue>
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

using integer = unsigned long;

template <typename T, typename... Args>
bool isAny(T val, Args... args) {
	return ((val == args) || ... );
}



// struct Scope {
// 	Sope* parent;
// 	bool isInside(Scope); // TODO
// }
// Scope* rootScope = nullptr;

struct Line {
	std::size_t row, col;
	std::string text;

	std::queue<std::vector<char>> strings;
	std::queue<char> characters;
//	Scope scope;
};

enum struct TokenType {
	   Identifier, Directive, Integer, String,
	// $       ,      :      `
	   Dollar, Comma, Colon, Tick,
	// =       (       )
	   Assign, Paren0, Paren1,
	// **   *     /    %    +     -
	   Exp, Mult, Div, Mod, Plus, Minus,
	// ==     !=      <   <=   >   >=
	   Equal, NEqual, LT, LTE, GT, GTE,
	// !     &&      ||
	   Excl, AmpAmp, BarBar,
	// ~      &    |    ^
	   Tilde, Amp, Bar, Hat,
	// <<     >>
	   LfArw, RtArw,
};

struct Token {
	TokenType type;
	std::string strValue;
	integer intValue = 0;
	Token(TokenType t) : type(t) {}
	Token(TokenType t, integer          val) : type(t), intValue(val) {}
	Token(TokenType t, std::string      val) : type(t), strValue(val) {}
	Token(TokenType t, std::string_view val) : type(t), strValue(val) {}
};

using TokenArray = std::vector<Token>;

bool holdsIntValue(TokenType t) {
	using enum TokenType;
	return isAny(t, Identifier, Integer, Dollar, Paren0, Paren1);
}

bool holdsStrValue(TokenType t) {
	using enum TokenType;
	return isAny(t, String);
}

bool holdsName(TokenType t) {
	using enum TokenType;
	return isAny(t, Identifier, Directive);
}

std::optional<Token> findToken(const auto& tokens, TokenType type) {
	auto result = ranges::find_if(tokens,
		[&](auto t) { return t.type == type; }
	);
	if (result != tokens.end()) { return *result; }
	return {};
};



constexpr int Invalid_Digit = -1;

using CharToNumber = int(char);
int digit(char c) {
	if ('0' <= c&&c <= '9') { return c-'0'; }
	return Invalid_Digit;
}
int hexDigit(char c) {
	if ('0' <= c&&c <= '9') { return c-'0'; }
	if ('A' <= c&&c <= 'F') { return c-'A'+10; }
	if ('a' <= c&&c <= 'f') { return c-'a'+10; }
	return Invalid_Digit;
}
int binDigit(char c) {
	if ('0' <= c&&c <= '1') { return c-'0'; }
	return Invalid_Digit;
}

// short-hand versions
using CharIsNumber = bool(char);
bool isDigit   (char c) { return digit(c)    != Invalid_Digit; }
bool isHexDigit(char c) { return hexDigit(c) != Invalid_Digit; }
bool isBinDigit(char c) { return binDigit(c) != Invalid_Digit; }
bool isAnyDigit(char c) { return isHexDigit(c); }



using CharPredicate = bool(char);
bool isWhitespace(char c) { return c==' ' || c=='\t' || c=='\n'; }
bool isLower (char c) { return 'a' <= c&&c <= 'z'; }
bool isUpper (char c) { return 'A' <= c&&c <= 'Z'; }
bool isLetter(char c) { return isLower(c) || isUpper(c); }

using CharTransformation = char(char);
char toLower(char c) { return isUpper(c) ? c + ('a'-'A') : c; }
char toUpper(char c) { return isLower(c) ? c + ('A'-'a') : c; }



#define DEBUG(msg) do {} while(0)
// #define DEBUG(msg)    do {std::cout << msg; } while(0)

int ErrorLineCount = -1;
void SetPrintLine(int ln) { ErrorLineCount = ln; }
void UnsetPrintLine()     { ErrorLineCount = -1; }

#define PrintWithLines(msg)                                            \
    if (ErrorLineCount > 0)                                            \
        std::cerr << msg << "\tNear line: " << ErrorLineCount << "\n"; \
    else                                                               \
        std::cerr << msg;                                               

#define  PrintStatus(msg) do { std::cerr << msg;                                        } while (0)
#define PrintWarning(msg) do { std::cerr << "Warning:\t"; PrintWithLines(msg);          } while (0)
#define   PrintError(msg) do { std::cerr << "Error:  \t"; PrintWithLines(msg); exit(1); } while (0)