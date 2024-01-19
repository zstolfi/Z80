#pragma once
#include "common.hh"
#include <istream>

namespace /*detail*/ {
	std::size_t  row = 1,  col = 1;
	[[maybe_unused]] int peek(std::istream& is) { return is.peek(); }
	[[maybe_unused]] int get (std::istream& is) {
		static bool eofReached = false;
		if (eofReached) { PrintError("syntax error at EOF\n"); }

		int c = is.get();
		if (c == '\n') { row++, col=1; } else { col++; }
		SetPrintLine(col);
		if (c == EOF) { eofReached = true; UnsetPrintLine(); }
		return c;
	}

	char parseChar(char, std::istream&);
}

// TODO: #define
// TODO: #definefn
// TODO: #undef
// TODO: #redef
// TODO: #include
// TODO: #incbin (include binary)
auto preprocess(std::istream& is) {
	std::vector<Line> result {};

	// https://files.catbox.moe/971dd5.png
	enum { Code, String, Char1, Char2, Comment, Accept } state = Code;

	Line line { row, col };
	std::vector<char> curString;
	for (char c=get(is); is; c=get(is)) {
		if (state == Accept) { // prev line accepted
			state = Code;
			result.push_back(line);
			line = { row, col };
		}

		if (state == Code) {
			DEBUG(row << "\t" << col << "\t'" << c << "'\tCode\n");
			switch (c) {
			case '"' : state = String;
			    curString = {};          continue;
			case '\'': state = Char1;    continue;
			case ';' : state = Comment;  continue;
			case '\n':
			case '\\': state = Accept;   continue;
			default:
			    line.text += toLower(c); continue;
		} }
		else if (state == String) {
			DEBUG(row << "\t" << col << "\t'" << c << "'\tString\n");
			if (c == '"') { state = Code;
				line.strings.push(curString);
				line.text += '"';
				continue; }
			curString.push_back(parseChar(c,is));
		}
		else if (state == Char1) {
			DEBUG(row << "\t" << col << "\t'" << c << "'\tChar1\n");
			if (c == '\'') { PrintError("empty char literal\n"); }
			line.characters.push(parseChar(c,is));
			state = Char2;
		}
		else if (state == Char2) {
			DEBUG(row << "\t" << col << "\t'" << c << "'\tChar2\n");
			if (c != '\'') { PrintError("multi-char literal\n"); }
				line.text += '\'';
			state = Code;
		}
		else if (state == Comment) {
			DEBUG(row << "\t" << col << "\t'" << c << "'\tComment\n");
			if (c == '\n') { state = Accept; }
		}
	}
	result.push_back(line);

	if (state != Accept
	&&  state != Code
	&&  state != Comment)
		PrintError("syntax error at EOF\n");
	
	return result;
}

namespace /*detail*/ {
	char parseChar(char c, std::istream& is) {
		if (c == '\n') { PrintError("multi-line string\n"); }
		if (c != '\\') { return c; }
		c = get(is);
		if (c == 'x') {
			char h1 = get(is);
			char h2 = get(is);
			if (!isHexDigit(h1) || !isHexDigit(h2))
				PrintError("invalid hex digit\n");
			return hexDigit(h1) << 4 | hexDigit(h2);
		}
		/**/ if (c=='0' ) { return '\0'; }
		else if (c=='a' ) { return '\a'; }
		else if (c=='b' ) { return '\b'; }
		else if (c=='e' ) { return '\\'; }
		else if (c=='f' ) { return '\f'; }
		else if (c=='n' ) { return '\n'; }
		else if (c=='r' ) { return '\r'; }
		else if (c=='t' ) { return '\t'; }
		else if (c=='v' ) { return '\v'; }
		else if (c=='\\') { return '\\'; }
		else if (c=='\'') { return '\''; }
		else if (c=='"' ) { return '"' ; }
		PrintError("unknown esc char\n"); return {};
	}
}