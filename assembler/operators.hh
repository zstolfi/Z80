#pragma once
#include "common.hh"

namespace /*detail*/ {
	integer pow(integer a, integer b) {
		integer result = 1;
		while (b--) { result *= a; }
		return result;
	}
}

namespace OP {
	enum Assoc { Left, Right };
	using UnaryFn  = integer(integer);
	using BinaryFn = integer(integer,integer);
	
	struct UnaryOpInfo {
		TokenType token;
		UnaryFn* fn;
		integer operator()(integer x) const { return fn(x); }
	};

	struct BinaryOpInfo {
		TokenType token;
		BinaryFn* fn;
		integer operator()(integer x, integer y) const { return fn(x,y); }
	};

	#define UnaryDef(Name,Symbol,Fn) \
		constexpr UnaryOpInfo Name {Symbol, [](integer a)->integer { return Fn; }}
	#define BinaryDef(Name,Symbol,Fn) \
		constexpr BinaryOpInfo Name {Symbol, [](integer a, integer b)->integer { return Fn; }}

	// Unary operators 
	UnaryDef (Pos    , TokenType::Plus   , +a      );
	UnaryDef (Neg    , TokenType::Minus  , -a      );
	UnaryDef (LogNot , TokenType::Excl   , !a      );
	UnaryDef (BitNot , TokenType::Tilde  , ~a      );
	// Arithmetic
	BinaryDef(Add    , TokenType::Plus   , a + b   );
	BinaryDef(Sub    , TokenType::Minus  , a - b   );
	BinaryDef(Mult   , TokenType::Mult   , a * b   );
	BinaryDef(Div    , TokenType::Div    , a / b   );
	BinaryDef(Mod    , TokenType::Mod    , a % b   );
	BinaryDef(Exp    , TokenType::Exp    , pow(a,b));
	// Comparison
	BinaryDef(Equal  , TokenType::Equal  , a == b );
	BinaryDef(NotEq  , TokenType::NEqual , a != b );
	BinaryDef(Les    , TokenType::LT     , a <  b );
	BinaryDef(LesEq  , TokenType::LTE    , a <= b );
	BinaryDef(Grt    , TokenType::GT     , a >  b );
	BinaryDef(GrtEq  , TokenType::GTE    , a >= b );
	// Logical
	BinaryDef(LogAnd , TokenType::AmpAmp , a && b );
	BinaryDef(LogOr  , TokenType::BarBar , a || b );
	// Bitwise
	BinaryDef(ShiftL , TokenType::LfArw  , a << b );
	BinaryDef(ShiftR , TokenType::RtArw  , a >> b );
	BinaryDef(BitAnd , TokenType::Amp    , a &  b );
	BinaryDef(BitOr  , TokenType::Bar    , a |  b );
	BinaryDef(BitXor , TokenType::Hat    , a ^  b );

	#undef UnaryDef
	#undef BinaryDef
}