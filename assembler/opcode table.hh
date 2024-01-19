#pragma once
#include <map>
#include "common.hh"
#include "bytes.hh"

// Z80 CPU User Manual pg. 39
enum struct ParamType { none, _ = none,
	A, HL, IX, IY,    // exaclt register names
	BC, DE, AF, F,    
	I, R, SP,         
	HL_d, IX_d, IY_d, // indirection
	IX_q, IY_q,       // (q = no offet)
	BC_d, DE_d, SP_d, 
	nn_d, n_d, C_d,   
	r,                //  8-bit reg. groups
	qq, ss, pp, rr,   // 16-bit reg. groups
	n, nn, d, b, e,   // number types
	cc, JRcc,         // conditions
	AF_p, IMn, RSTn,  // misc.
	n_0,              
};
using ParamVal = int;

namespace /*detail*/ {
	using enum ParamType;
	using OpFn = std::vector<std::byte> (ParamVal, ParamVal);

	struct OpCode {
		ParamType pt0;
		ParamType pt1;
		std::size_t size;
		OpFn* fn;
		auto operator()(ParamVal x, ParamVal y) const { return fn(x,y); }
	};

	template <typename... Args>
	auto BytesVec(Args... args) {
	    auto arr = std::array{ std::byte(args)... };
	    return std::vector(arr.begin(), arr.end());
	}
}

static const std::map<
	ParamType,
	std::vector<std::string_view>
> ParamValTable {
	{A,{"a"}}, {I,{"i"}}, {R,{"r"}}, {SP,{"sp"}},
	{HL,{"hl"}}, {IX,{"ix"}}, {IY,{"iy"}},
	{BC,{"bc"}}, {DE,{"de"}}, {AF,{"af"}},
	//     order very matters on the vectors
	{r   , {"b","c","d","e","h","l","","a"}},
	{qq  , {"bc","de","hl","af"}},
	{ss  , {"bc","de","hl","sp"}},
	{pp  , {"bc","de","ix","sp"}},
	{rr  , {"bc","de","iy","sp"}},
	{cc  , {"nz","z","nc","c","po","pe","p","m"}},
	{JRcc, {"nz","z","nc","c"}}//,
},

ParamValTable_d {
	{HL_d, {"hl"}}, {SP_d, {"sp"}},
	{IX_q, {"ix"}}, {IY_q, {"iy"}},
	{BC_d, {"bc"}}, {DE_d, {"de"}},
	{C_d , {"c" }}//,
};

const bool isRegisterName(std::string_view name) {
	for (auto& [ _ , names] : ParamValTable) {
		for (auto& n : names) {
			if (n == name) { return true; }
	} }
	return false;
}

static const std::map<
	ParamType,
	bool(*)(ParamVal)
> NumberParamTypes {
	{n   , [](ParamVal i) { return 0 <= i&&i <= 255;   } },
	{n_d , [](ParamVal i) { return 0 <= i&&i <= 255;   } },
	{nn  , [](ParamVal i) { return 0 <= i&&i <= 65535; } },
	{nn_d, [](ParamVal i) { return 0 <= i&&i <= 65535; } },
	{b   , [](ParamVal i) { return 0 <= i&&i <= 7;     } },
	{d   , [](ParamVal i) { return -128 <= i&&i <= 127;} },
	{e   , [](ParamVal i) { return -126 <= i&&i <= 129;} },
	{IMn , [](ParamVal i) { return 0 <= i&&i < 3;      } },
	{RSTn, [](ParamVal i) { return i<64 && i%8 == 0;   } },
	{n_0 , [](ParamVal i) { return i == 0;             } }//,
};

const bool validNumberParam(ParamType type, ParamVal i) {
	if (!NumberParamTypes.contains(type)) { return {}; }
	return NumberParamTypes.at(type)(i);
}

// Z80 CPU User Manual pg. 69
const std::multimap<std::string_view, OpCode> OpCodeTable  {
	#define BV(A,B,...) [](auto A, auto B) { return BytesVec(__VA_ARGS__); }
	// 8-Bit Load Group
	{{"ld"  }, { r   , r   , 1, BV(r0, r1, 0x40|r0<<3|r1     ) }},
	{{"ld"  }, { r   , n   , 2, BV(r , n , 0x06|r<<3, n      ) }},
	{{"ld"  }, { r   , HL_d, 1, BV(r ,   , 0x46|r<<3         ) }},
	{{"ld"  }, { r   , IX_d, 3, BV(r , d , 0xDD, 0x46|r<<3, d) }},
	{{"ld"  }, { r   , IY_d, 3, BV(r , d , 0xFD, 0x46|r<<3, d) }},
	{{"ld"  }, { HL_d, r   , 1, BV(  , r , 0x70|r            ) }},
	{{"ld"  }, { IX_d, r   , 3, BV(d , r , 0xDD, 0x70|r, d   ) }},
	{{"ld"  }, { IY_d, r   , 3, BV(d , r , 0xFD, 0x70|r, d   ) }},
	{{"ld"  }, { HL_d, n   , 2, BV(  , n , 0x36, n           ) }},
	{{"ld"  }, { IX_d, n   , 4, BV(d , n , 0xDD, 0x36, d, n  ) }},
	{{"ld"  }, { IY_d, n   , 4, BV(d , n , 0xFD, 0x36, d, n  ) }},
	{{"ld"  }, { A   , BC_d, 1, BV(  ,   , 0x0A              ) }},
	{{"ld"  }, { A   , DE_d, 1, BV(  ,   , 0x1A              ) }},
	{{"ld"  }, { A   , nn_d, 3, BV(  , nn, 0x3A, nn, nn>>8   ) }},
	{{"ld"  }, { BC_d, A   , 1, BV(  ,   , 0x02              ) }},
	{{"ld"  }, { DE_d, A   , 1, BV(  ,   , 0x12              ) }},
	{{"ld"  }, { nn_d, A   , 3, BV(nn,   , 0x32, nn, nn>>8   ) }},
	{{"ld"  }, { A   , I   , 2, BV(  ,   , 0xED, 0x57        ) }},
	{{"ld"  }, { A   , R   , 2, BV(  ,   , 0xED, 0x5F        ) }},
	{{"ld"  }, { I   , A   , 2, BV(  ,   , 0xED, 0x47        ) }},
	{{"ld"  }, { R   , A   , 2, BV(  ,   , 0xED, 0x4F        ) }},
	// 16-Bit Load Group
	{{"ld"  }, { ss  , nn  , 3, BV(ss, nn, 0x01|ss<<4, nn, nn>>8      ) }},
	{{"ld"  }, { IX  , nn  , 4, BV(  , nn, 0xDD, 0x21, nn, nn>>8      ) }},
	{{"ld"  }, { IY  , nn  , 4, BV(  , nn, 0xFD, 0x21, nn, nn>>8      ) }},
	{{"ld"  }, { HL  , nn_d, 3, BV(  , nn, 0x2A, nn, nn>>8            ) }},
	{{"ld"  }, { ss  , nn_d, 4, BV(ss, nn, 0xED, 0x4B|ss<<4, nn, nn>>8) }},
	{{"ld"  }, { IX  , nn_d, 4, BV(  , nn, 0xDD, 0x2A, nn, nn>>8      ) }},
	{{"ld"  }, { IY  , nn_d, 4, BV(  , nn, 0xFD, 0x2A, nn, nn>>8      ) }},
	{{"ld"  }, { nn_d, HL  , 3, BV(nn,   , 0x22, nn, nn>>8            ) }},
	{{"ld"  }, { nn_d, ss  , 4, BV(nn, ss, 0xED, 0x43|ss<<4, nn, nn>>8) }},
	{{"ld"  }, { nn_d, IX  , 4, BV(nn,   , 0xDD, 0x22, nn, nn>>8      ) }},
	{{"ld"  }, { nn_d, IY  , 4, BV(nn,   , 0xFD, 0x22, nn, nn>>8      ) }},
	{{"ld"  }, { SP  , HL  , 1, BV(  ,   , 0xF9                       ) }},
	{{"ld"  }, { SP  , IX  , 2, BV(  ,   , 0xDD, 0xF9                 ) }},
	{{"ld"  }, { SP  , IY  , 2, BV(  ,   , 0xFD, 0xF9                 ) }},
	{{"push"}, { qq  , _   , 1, BV(qq,   , 0xC5|qq<<4                 ) }},
	{{"push"}, { IX  , _   , 2, BV(  ,   , 0xDD, 0xE5                 ) }},
	{{"push"}, { IY  , _   , 2, BV(  ,   , 0xFD, 0xE5                 ) }},
	{{"pop" }, { qq  , _   , 1, BV(qq,   , 0xC1|qq<<4                 ) }},
	{{"pop" }, { IX  , _   , 2, BV(  ,   , 0xDD, 0xE1                 ) }},
	{{"pop" }, { IY  , _   , 2, BV(  ,   , 0xFD, 0xE1                 ) }},
	// Exchange, Block Transfer, and Search Group
	{{"ex"  }, { DE  , HL  , 1, BV(  ,   , 0xEB      ) }},
	{{"ex"  }, { AF  , AF_p, 1, BV(  ,   , 0x08      ) }},
	{{"exx" }, { _   , _   , 1, BV(  ,   , 0xD9      ) }},
	{{"ex"  }, { SP_d, HL  , 1, BV(  ,   , 0xE3      ) }},
	{{"ex"  }, { SP_d, IX  , 2, BV(  ,   , 0xDD, 0xE3) }},
	{{"ex"  }, { SP_d, IY  , 2, BV(  ,   , 0xFD, 0xE3) }},
	{{"ldi" }, { _   , _   , 2, BV(  ,   , 0xED, 0xA0) }},
	{{"ldir"}, { _   , _   , 2, BV(  ,   , 0xED, 0xB0) }},
	{{"ldd" }, { _   , _   , 2, BV(  ,   , 0xED, 0xA8) }},
	{{"lddr"}, { _   , _   , 2, BV(  ,   , 0xED, 0xB8) }},
	{{"cpi" }, { _   , _   , 2, BV(  ,   , 0xED, 0xA1) }},
	{{"cpir"}, { _   , _   , 2, BV(  ,   , 0xED, 0xB1) }},
	{{"cpd" }, { _   , _   , 2, BV(  ,   , 0xED, 0xA9) }},
	{{"cpdr"}, { _   , _   , 2, BV(  ,   , 0xED, 0xB9) }},
	// 8-Bit Arithmetic Group
	{{"add" }, { A   , r   , 1, BV(  , r , 0x80|r       ) }},
	{{"add" }, { A   , n   , 2, BV(  , n , 0xC6, n      ) }},
	{{"add" }, { A   , HL_d, 1, BV(  ,   , 0x86         ) }},
	{{"add" }, { A   , IX_d, 3, BV(  , d , 0xDD, 0x86, d) }},
	{{"add" }, { A   , IY_d, 3, BV(  , d , 0xFD, 0x86, d) }},
	{{"adc" }, { A   , r   , 1, BV(  , r , 0x88|r       ) }},
	{{"adc" }, { A   , n   , 2, BV(  , n , 0xCE, n      ) }},
	{{"adc" }, { A   , HL_d, 1, BV(  ,   , 0x8E         ) }},
	{{"adc" }, { A   , IX_d, 3, BV(  , d , 0xDD, 0x8E, d) }},
	{{"adc" }, { A   , IY_d, 3, BV(  , d , 0xFD, 0x8E, d) }},
	{{"sub" }, { r   , _   , 1, BV(r ,   , 0x90|r       ) }},
	{{"sub" }, { n   , _   , 2, BV(n ,   , 0xD6, n      ) }},
	{{"sub" }, { HL_d, _   , 1, BV(  ,   , 0x96         ) }},
	{{"sub" }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0x96, d) }},
	{{"sub" }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0x96, d) }},
	{{"sbc" }, { A   , r   , 1, BV(  , r , 0x98|r       ) }},
	{{"sbc" }, { A   , n   , 2, BV(  , n , 0xDE, n      ) }},
	{{"sbc" }, { A   , HL_d, 1, BV(  ,   , 0x9E         ) }},
	{{"sbc" }, { A   , IX_d, 3, BV(  , d , 0xDD, 0x9E, d) }},
	{{"sbc" }, { A   , IY_d, 3, BV(  , d , 0xFD, 0x9E, d) }},
	{{"and" }, { r   , _   , 1, BV(r ,   , 0xA0|r       ) }},
	{{"and" }, { n   , _   , 2, BV(n ,   , 0xE6, n      ) }},
	{{"and" }, { HL_d, _   , 1, BV(  ,   , 0xA6         ) }},
	{{"and" }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0xA6, d) }},
	{{"and" }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0xA6, d) }},
	{{"or"  }, { r   , _   , 1, BV(r ,   , 0xB0|r       ) }},
	{{"or"  }, { n   , _   , 2, BV(n ,   , 0xF6, n      ) }},
	{{"or"  }, { HL_d, _   , 1, BV(  ,   , 0xB6         ) }},
	{{"or"  }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0xB6, d) }},
	{{"or"  }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0xB6, d) }},
	{{"xor" }, { r   , _   , 1, BV(r ,   , 0xA8|r       ) }},
	{{"xor" }, { n   , _   , 2, BV(n ,   , 0xEE, n      ) }},
	{{"xor" }, { HL_d, _   , 1, BV(  ,   , 0xAE         ) }},
	{{"xor" }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0xAE, d) }},
	{{"xor" }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0xAE, d) }},
	{{"cp"  }, { r   , _   , 1, BV(r ,   , 0xB8|r       ) }},
	{{"cp"  }, { n   , _   , 2, BV(n ,   , 0xFE, n      ) }},
	{{"cp"  }, { HL_d, _   , 1, BV(  ,   , 0xBE         ) }},
	{{"cp"  }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0xBE, d) }},
	{{"cp"  }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0xBE, d) }},
	{{"inc" }, { r   , _   , 1, BV(r ,   , 0x04|r<<3    ) }},
	{{"inc" }, { HL_d, _   , 1, BV(  ,   , 0x34         ) }},
	{{"inc" }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0x34, d) }},
	{{"inc" }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0x34, d) }},
	{{"dec" }, { r   , _   , 1, BV(r ,   , 0x05|r<<3    ) }},
	{{"dec" }, { HL_d, _   , 1, BV(  ,   , 0x35         ) }},
	{{"dec" }, { IX_d, _   , 3, BV(d ,   , 0xDD, 0x35, d) }},
	{{"dec" }, { IY_d, _   , 3, BV(d ,   , 0xFD, 0x35, d) }},
	// General-Purpose Arithmetic and CPU Control Groups
	{{"daa" }, { _   , _   , 1, BV(  ,   , 0x27      ) }},
	{{"cpl" }, { _   , _   , 1, BV(  ,   , 0x2F      ) }},
	{{"neg" }, { _   , _   , 2, BV(  ,   , 0xED, 0x44) }},
	{{"ccf" }, { _   , _   , 1, BV(  ,   , 0x3F      ) }},
	{{"scf" }, { _   , _   , 1, BV(  ,   , 0x37      ) }},
	{{"nop" }, { _   , _   , 1, BV(  ,   , 0x00      ) }},
	{{"halt"}, { _   , _   , 1, BV(  ,   , 0x76      ) }},
	{{"di"  }, { _   , _   , 1, BV(  ,   , 0xF3      ) }},
	{{"ei"  }, { _   , _   , 1, BV(  ,   , 0xFB      ) }},
	{{"im"  }, { IMn , _   , 2, BV(i ,   , 0xED, (int[]){70,86,94}[i]) }},
	// 16-Bit Arithmetic Group
	{{"add" }, { HL  , ss  , 1, BV(  , ss, 0x09|ss<<4      ) }},
	{{"adc" }, { HL  , ss  , 2, BV(  , ss, 0xED, 0x4A|ss<<4) }},
	{{"sbc" }, { HL  , ss  , 2, BV(  , ss, 0xED, 0x42|ss<<4) }},
	{{"add" }, { IX  , pp  , 2, BV(  , pp, 0xDD, 0x09|pp<<4) }},
	{{"add" }, { IY  , rr  , 2, BV(  , rr, 0xFD, 0x09|rr<<4) }},
	{{"inc" }, { ss  , _   , 1, BV(ss,   , 0x03|ss<<4      ) }},
	{{"inc" }, { IX  , _   , 2, BV(  ,   , 0xDD, 0x23      ) }},
	{{"inc" }, { IY  , _   , 2, BV(  ,   , 0xFD, 0x23      ) }},
	{{"dec" }, { ss  , _   , 1, BV(ss,   , 0x0B|ss<<4      ) }},
	{{"dec" }, { IX  , _   , 2, BV(  ,   , 0xDD, 0x2B      ) }},
	{{"dec" }, { IY  , _   , 2, BV(  ,   , 0xFD, 0x2B      ) }},
	// Rotate and Shift Group
	{{"rlca"}, { _   , _   , 1, BV(  ,   , 0x07               ) }},
	{{"rla" }, { _   , _   , 1, BV(  ,   , 0x17               ) }},
	{{"rrca"}, { _   , _   , 1, BV(  ,   , 0x0F               ) }},
	{{"rra" }, { _   , _   , 1, BV(  ,   , 0x1F               ) }},
	{{"rlc" }, { r   , _   , 2, BV(r ,   , 0xCB, 0x00|r       ) }},
	{{"rlc" }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x06         ) }},
	{{"rlc" }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x06) }},
	{{"rlc" }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x06) }},
	{{"rl"  }, { r   , _   , 2, BV(r ,   , 0xCB, 0x10|r       ) }},
	{{"rl"  }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x16         ) }},
	{{"rl"  }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x16) }},
	{{"rl"  }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x16) }},
	{{"rrc" }, { r   , _   , 2, BV(r ,   , 0xCB, 0x08|r       ) }},
	{{"rrc" }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x0E         ) }},
	{{"rrc" }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x0E) }},
	{{"rrc" }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x0E) }},
	{{"rr"  }, { r   , _   , 2, BV(r ,   , 0xCB, 0x18|r       ) }}, // pg. 228 has a mistake
	{{"rr"  }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x1E         ) }},
	{{"rr"  }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x1E) }},
	{{"rr"  }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x1E) }},
	{{"sla" }, { r   , _   , 2, BV(r ,   , 0xCB, 0x20|r       ) }},
	{{"sla" }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x26         ) }},
	{{"sla" }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x26) }},
	{{"sla" }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x26) }},
	{{"sll" }, { r   , _   , 2, BV(r ,   , 0xCB, 0x30|r       ) }}, // undocumented
	{{"sll" }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x36         ) }}, // undocumented
	{{"sll" }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x36) }}, // undocumented
	{{"sll" }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x36) }}, // undocumented
	{{"sra" }, { r   , _   , 2, BV(r ,   , 0xCB, 0x28|r       ) }},
	{{"sra" }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x2E         ) }},
	{{"sra" }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x2E) }},
	{{"sra" }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x2E) }},
	{{"srl" }, { r   , _   , 2, BV(r ,   , 0xCB, 0x38|r       ) }},
	{{"srl" }, { HL_d, _   , 2, BV(  ,   , 0xCB, 0x3E         ) }},
	{{"srl" }, { IX_d, _   , 4, BV(d ,   , 0xDD, 0xCB, d, 0x3E) }},
	{{"srl" }, { IY_d, _   , 4, BV(d ,   , 0xFD, 0xCB, d, 0x3E) }},
	{{"rld" }, { _   , _   , 2, BV(  ,   , 0xED, 0x6F         ) }},
	{{"rrd" }, { _   , _   , 2, BV(  ,   , 0xED, 0x67         ) }},
	// Bit Set, Reset, and Test Group
	{{"bit" }, { b   , r   , 2, BV(b , r , 0xCB, 0x40|b<<3|r       ) }},
	{{"bit" }, { b   , HL_d, 2, BV(b ,   , 0xCB, 0x46|b<<3         ) }},
	{{"bit" }, { b   , IX_d, 4, BV(b , d , 0xDD, 0xCB, d, 0x46|b<<3) }},
	{{"bit" }, { b   , IY_d, 4, BV(b , d , 0xFD, 0xCB, d, 0x46|b<<3) }},
	{{"set" }, { b   , r   , 2, BV(b , r , 0xCB, 0xC0|b<<3|r       ) }},
	{{"set" }, { b   , HL_d, 2, BV(b ,   , 0xCB, 0xC6|b<<3         ) }},
	{{"set" }, { b   , IX_d, 4, BV(b , d , 0xDD, 0xCB, d, 0xC6|b<<3) }},
	{{"set" }, { b   , IY_d, 4, BV(b , d , 0xFD, 0xCB, d, 0xC6|b<<3) }},
	{{"res" }, { b   , r   , 2, BV(b , r , 0xCB, 0x80|b<<3|r       ) }},
	{{"res" }, { b   , HL_d, 2, BV(b ,   , 0xCB, 0x86|b<<3         ) }},
	{{"res" }, { b   , IX_d, 4, BV(b , d , 0xDD, 0xCB, d, 0x86|b<<3) }},
	{{"res" }, { b   , IY_d, 4, BV(b , d , 0xFD, 0xCB, d, 0x86|b<<3) }},
	// Jump Group
	{{"jp"  }, { nn  , _   , 3, BV(nn,   , 0xC3, nn, nn>>8      ) }},
	{{"jp"  }, { cc  , nn  , 3, BV(cc, nn, 0xC2|cc<<3, nn, nn>>8) }},
	{{"jr"  }, { e   , _   , 2, BV(e ,   , 0x18, e-2            ) }},
	{{"jr"  }, { JRcc, e   , 2, BV(cc, e , 0x20|cc<<3, e-2      ) }},
	{{"jp"  }, { HL_d, _   , 1, BV(  ,   , 0xE9                 ) }},
	{{"jp"  }, { IX_q, _   , 2, BV(  ,   , 0xDD, 0xE9           ) }},
	{{"jp"  }, { IY_q, _   , 2, BV(  ,   , 0xFD, 0xE9           ) }},
	{{"djnz"}, { e   , _   , 2, BV(e ,   , 0x10, e-2            ) }},
	// Call and Return Group
	{{"call"}, { nn  , _   , 3, BV(nn,   , 0xCD, nn, nn>>8      ) }},
	{{"call"}, { cc  , nn  , 3, BV(cc, nn, 0xC4|cc<<3, nn, nn>>8) }},
	{{"ret" }, { _   , _   , 1, BV(  ,   , 0xC9                 ) }},
	{{"ret" }, { cc  , _   , 1, BV(cc,   , 0xC0|cc<<3           ) }},
	{{"reti"}, { _   , _   , 2, BV(  ,   , 0xED, 0x4D           ) }},
	{{"retn"}, { _   , _   , 2, BV(  ,   , 0xED, 0x45           ) }},
	{{"rst" }, { RSTn, _   , 1, BV(n ,   , 0xC7|n               ) }},
	// Input and Output Group
	{{"in"  }, { A   , n_d , 2, BV(  , n , 0xDB, n        ) }},
	{{"in"  }, { r   , C_d , 2, BV(r ,   , 0xED, 0x40|r<<3) }},
	{{"in"  }, { F   , C_d , 2, BV(  ,   , 0xED, 0x70     ) }}, // undocumented
	{{"in"  }, { C_d , _   , 2, BV(  ,   , 0xED, 0x70     ) }}, // undocumented
	{{"ini" }, { _   , _   , 2, BV(  ,   , 0xED, 0xA2     ) }},
	{{"inir"}, { _   , _   , 2, BV(  ,   , 0xED, 0xB2     ) }},
	{{"ind" }, { _   , _   , 2, BV(  ,   , 0xED, 0xAA     ) }},
	{{"indr"}, { _   , _   , 2, BV(  ,   , 0xED, 0xBA     ) }},
	{{"out" }, { n_d , A   , 2, BV(n ,   , 0xD3, n        ) }},
	{{"out" }, { C_d , r   , 2, BV(  , r , 0xED, 0x41|r<<3) }},
	{{"out" }, { C_d , n_0 , 2, BV(  ,   , 0xED, 0x71     ) }}, // undocumented
	{{"outi"}, { _   , _   , 2, BV(  ,   , 0xED, 0xA3     ) }},
	{{"otir"}, { _   , _   , 2, BV(  ,   , 0xED, 0xB3     ) }},
	{{"outd"}, { _   , _   , 2, BV(  ,   , 0xED, 0xAB     ) }},
	{{"otdr"}, { _   , _   , 2, BV(  ,   , 0xED, 0xBB     ) }}//,
	#undef BV
};



// Z80 CPU User Manual can be found here:
//     https://www.zilog.com/docs/z80/um0080.pdf