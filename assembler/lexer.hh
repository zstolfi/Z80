#pragma once
#include "common.hh"
using namespace std::literals;

namespace /*detail*/ {
	bool    isInteger   (std::string_view);
	integer parseInteger(std::string_view);
}

TokenArray lexLine(Line& line) {
	TokenArray result {};

	std::string_view str(line.text);
	auto sub = [=](auto i, auto size) { return str.substr(i,size); };

	for (std::size_t i=0, size=1; i+size <= str.size(); i+=size, size=1) {
		// whitespace
		if (isWhitespace(str[i])) { continue; }
		// identifier
		if (isLetter(str[i])) {
			while (isLetter(str[i+size]) || isDigit(str[i+size]))
				size++;
			result.emplace_back(TokenType::Identifier, sub(i,size));
			continue;
		}
		// integer
		if (isInteger(sub(i,size=1))
		||  isInteger(sub(i,size=2))) {
			while (isInteger(sub(i,size+1)) && i+size+1 <= str.size())
				size++;
			integer intVal = parseInteger(sub(i,size)); // parse integers at lex time
			result.emplace_back(TokenType::Integer, intVal);
			continue;
		} size = 1;
		// directive
		if (str[i] == '.') {
			while (isLetter(str[i+size]))
				size++;
			if (size > 1) {
				if (auto dir = sub(i+1,size-1); dir == "equ"sv) {
					result.emplace_back(TokenType::Assign);
					continue;
				} else {
					result.emplace_back(TokenType::Directive, dir);
					continue;
			} }
			size = 1;
		}
		// string literal
		if (str[i] == '\"') {
			auto& vec = line.strings.front();
			auto stringVal = std::string{vec.begin(), vec.end()};
			result.emplace_back(TokenType::String, stringVal);
			line.strings.pop(); continue;
		}
		// character literal
		if (str[i] == '\'') {
			integer intVal = line.characters.front();
			result.emplace_back(TokenType::Integer, intVal);
			line.characters.pop(); continue;
		}

		#define Match(S,TT)                       \
		if (sub(i,size = sizeof(S)-1) == S##sv) { \
		    result.emplace_back(TT);              \
		    continue;                             \
		}                                          
		
		Match("==" , TokenType::Equal );
		Match("="  , TokenType::Assign);
		Match("**" , TokenType::Exp   );
		Match("*"  , TokenType::Mult  );
		Match("&&" , TokenType::AmpAmp);
		Match("&"  , TokenType::Amp   );
		Match("||" , TokenType::BarBar);
		Match("|"  , TokenType::Bar   );
		Match("<<" , TokenType::LfArw );
		Match("<=" , TokenType::LTE   );
		Match("<"  , TokenType::LT    );
		Match(">>" , TokenType::RtArw );
		Match(">=" , TokenType::GTE   );
		Match(">"  , TokenType::GT    );
		Match("!=" , TokenType::NEqual);
		Match("!"  , TokenType::Excl  );
		Match("$"  , TokenType::Dollar);
		Match("/"  , TokenType::Div   );
		Match("%"  , TokenType::Mod   );
		Match("+"  , TokenType::Plus  );
		Match("-"  , TokenType::Minus );
		Match("~"  , TokenType::Tilde );
		Match("^"  , TokenType::Hat   );
		Match(","  , TokenType::Comma );
		Match(":"  , TokenType::Colon );
		Match("`"  , TokenType::Tick );
		Match("("  , TokenType::Paren0);
		Match(")"  , TokenType::Paren1);
		PrintWarning("unknown token\n");

		#undef Match
	}
	return result;
}

auto lex(std::vector<Line>& lines) {
	std::vector<TokenArray> result {};

	int lineCount = 0;
	for (auto it=lines.begin(); it!=lines.end(); ++it) {
		SetPrintLine(++lineCount);
		if (!ranges::all_of(it->text, isWhitespace))
			result.push_back(lexLine(*it));
	}
	UnsetPrintLine();

	return result;
}

namespace /*detail*/ {
	bool isInteger(std::string_view str) {
		if (str.empty()) { return false; }
		char prefix = str.front(), suffix = str.back();

		if ("#$%"sv.contains(prefix))
			str.remove_prefix(1);

		if ((prefix=='#' && (suffix=='h' || suffix=='b'))
		||  (prefix=='%' && (suffix=='d' || suffix=='h')))
			PrintError("mismatched base prefix/suffix in integer\n");

		if ("dhb"sv.contains(suffix) && prefix != '$')
			str.remove_suffix(1);

		return str.size() && ranges::all_of(str, isAnyDigit);
	}

	integer parseInteger(std::string_view str) {
		integer result = 0;

		// Octal will not supported, because evil
		// number systems are not to be tolerated
		enum Radix { Dec=10, Hex=16, Bin=2 };
		Radix base = Dec;

		switch (str.back()) {
			case 'd': base = Dec; break;
			case 'h': base = Hex; break;
			case 'b': base = Bin; break;
		}

		std::size_t i=0;
		char c = str[i];

		/**/ if (c=='#') { base = Dec; c=str[++i]; }
		else if (c=='$') { base = Hex; c=str[++i]; }
		else if (c=='%') { base = Bin; c=str[++i]; }

		CharToNumber& digitB =
			  (base==Dec) ? digit
			: (base==Hex) ? hexDigit
			:/*base==Bin*/  binDigit;

		if (digitB(c) == Invalid_Digit)
			PrintError("incorrect base in integer\n");

		while (digitB(c=str[i++]) != Invalid_Digit)
			result = base*result + digitB(c);

		return result;
	}
}