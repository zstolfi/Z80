#pragma once
#include "common.hh"
#include "bytes.hh"
#include "operators.hh"
#include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>
#include <set>
#include <variant>
using namespace std::literals;

namespace OP {
	// Order of Operations table! Modify
	// it for the parser of your choice.
	constexpr std::tuple Order {
		std::pair{Right, std::array{Pos, Neg}},
		std::pair{Right, std::array{Exp}},
		std::pair{Left , std::array{Mult, Div}},
		std::pair{Left , std::array{Add, Sub}},
	};
}

namespace /*detail*/ {
	struct Context {
		std::size_t progCounter = 0;
		std::map<std::string_view,integer> vars;
		void setVariable(std::string_view, integer);
		integer getVariable(std::string_view);
	};

	std::set<ParamType> getParamTypes(std::span<Token>);
	// All possible ways a parameter can be interpreted
	// i.e. 128 => {n, nn, e}
	//      A   => {A, r}
	ParamVal getParamVal(Context&, ParamType, std::span<Token>);


	using Grouping = std::vector<std::size_t>;
	// Describes "grouping" of tokens
	// i.e. 1 * ( 4 + 2 )
	// =>   0 1 6 3 4 5 2

	struct ExpressionContext {
	// You know when you use PEMDAS to simplify
	// a math equation, and you show your work?
	// This class is that work.
		std::span<Token> tokens;
		Grouping groups;
		std::vector<integer> evaluated;
		ExpressionContext(std::span<Token>);
	};

	integer parseExpression(Context&, std::span<Token>);
	std::vector<std::span<Token>> splitArgs(std::span<Token>);

	template <OP::Assoc, typename Fn>
	void iterateGroups(const Grouping&, Fn);
	template <typename Fn>
	void iterateGroups(const Grouping&, Fn);
	template <OP::Assoc Dir, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext&, std::array<Fn,N>);
}

// assembler directives:
//     .org  .origin (set program counter)
//     .db   .byte   (byte literals)
//     .dw   .word   (word literals)
//     .ds   .space  (reserve bytes)
//     .equ  =       (const assignment)
//     .end          (stop parsing)

auto parse(std::vector<TokenArray>& lines) {
	std::vector<std::byte> result {};
	// TODO: Basic syntax checks
	Context ctx;

	// pass 1: evaluate all label addresses and put them into context
	//         also queue every opcode / param (and reserve its size)
	PrintStatus("Pass 1:\n");

	struct Statement {
		std::size_t address;
		const OpCode& op;
		std::span<Token> param0;
		std::span<Token> param1;
	};
	using Data = std::vector<std::byte>;

	std::queue<
		std::variant<Statement, Data>
	> statementQueue;

	int lineCount = 0;
	for (TokenArray& line : lines) {
		SetPrintLine(++lineCount);
		PrintStatus(lineCount << "\t");

		// directives
		if (line[0].type == TokenType::Directive) {
			if (isAny(line[0].strValue, "org","origin")) {
				ctx.progCounter = parseExpression(ctx, {++line.begin(), line.end()});
				continue;
			}
			else if (isAny(line[0].strValue, "db","byte")) {
				auto params = splitArgs({++line.begin(), line.end()});
				for (auto param : params) {
					integer val = parseExpression(ctx, param);
					auto b = Bytes(val);
					statementQueue.push(std::vector(b.begin(), b.end()));
					ctx.progCounter += b.size();
				}
				continue;
			}
			else if (isAny(line[0].strValue, "dw","word")) {
				auto params = splitArgs({++line.begin(), line.end()});
				for (auto param : params) {
					integer val = parseExpression(ctx, param);
					auto b = Bytes<Word::LSB>(val);
					statementQueue.push(std::vector(b.begin(), b.end()));
					ctx.progCounter += b.size();
				}
				continue;
			}
			else if (isAny(line[0].strValue, "ds","space")) {
				auto params = splitArgs({++line.begin(), line.end()});
				integer len=0, dat=0;
				if (params.size() >= 1) { len = parseExpression(ctx, params[0]); }
				if (params.size() >= 2) { dat = parseExpression(ctx, params[1]); }
				if (params.size() >= 3) { PrintWarning("too many .ds arguments"); }
				if (params.size() == 0) { PrintWarning(".ds arguments expected"); }
				auto b = std::vector<std::byte>(len, std::byte(dat));
				statementQueue.push(b);
				ctx.progCounter += len;
				continue;
			}
			else if (isAny(line[0].strValue, "end")) { break; }

			PrintWarning("unknown directive");
		}

		std::size_t i=0;
		// labels
		while (i+2 <= line.size()
		&&     line[i+0].type == TokenType::Identifier
		&&     line[i+1].type == TokenType::Colon) {
			ctx.setVariable(line[i].strValue, ctx.progCounter);
			i += 2;
		}

		if (i >= line.size()) { continue; }

		if (line[i].type == TokenType::Identifier) {
			// assignment
			if (line.size()-i >= 3 && line[i+1].type == TokenType::Assign) {
				auto& varName = line[i].strValue;
				std::span s {std::next(line.begin(),2), line.end()};
				ctx.setVariable(varName, parseExpression(ctx, s));
				continue;
			}
			// instruction
			else if (OpCodeTable.contains(line[i].strValue)) {
				auto args = splitArgs({++line.begin(), line.end()});
				std::set<ParamType> paramTypes0, paramTypes1;
				paramTypes0 = paramTypes1 = { ParamType::none };
				if (args.size() >= 1) { paramTypes0 = getParamTypes(args[0]); }
				if (args.size() >= 2) { paramTypes1 = getParamTypes(args[1]); }
				if (args.size() >= 3) { PrintWarning("too many opcode arguments\n"); }
				args.resize(2);

				PrintStatus("{");
				for (auto p : paramTypes0)
					PrintStatus(" " << (int)p);
				PrintStatus(" } {");
				for (auto p : paramTypes1)
					PrintStatus(" " << (int)p);
				PrintStatus(" }\t" << line[i].strValue << "\t");

				auto [begin,end] = OpCodeTable.equal_range(line[i].strValue);
				auto it = std::find_if(begin, end,
					[&](auto pair) { auto op = pair.second;
						return paramTypes0.contains(op.pt0)
						&&     paramTypes1.contains(op.pt1);
					}
				);
				if (it == end) { PrintError("unknown opcode arguments"); }
				const OpCode& op = it->second;

				PrintStatus((int)op.pt0 << " " << (int)op.pt1 << "\n");
				statementQueue.push(Statement{
					ctx.progCounter, op, args[0], args[1]
				});
				ctx.progCounter += op.size;
				continue;
			}
			PrintError("unkown opcode: " << line[i].strValue);
		}
	}
	UnsetPrintLine();


	// pass 2: evaluate all statements
	PrintStatus("Pass 2:\n");

	lineCount = 0;
	for (; !statementQueue.empty(); statementQueue.pop()) {
		SetPrintLine(++lineCount);
		auto i = statementQueue.front();
		
		std::vector<std::byte> bytes;
		// Data on the queue
		if (auto* data = std::get_if<Data>(&i))
			bytes = *data;
		// OpCode on the queue
		if (auto* statement = std::get_if<Statement>(&i)) {
			ctx.progCounter = statement->address;
			ParamVal p0 = getParamVal(ctx, statement->op.pt0, statement->param0);
			ParamVal p1 = getParamVal(ctx, statement->op.pt1, statement->param1);
			bytes = statement->op(p0,p1);
		}

		result.insert(result.end(), bytes.begin(), bytes.end());
	}
	UnsetPrintLine();

	return result;
}

namespace /*detail*/ {
	void Context::setVariable(std::string_view name, integer val) {
		if (vars.contains(name))
			PrintError("redeclared variable: " << name);
		vars[name] = val;
	}

	integer Context::getVariable(std::string_view name) {
		if (!vars.contains(name))
			PrintError("undeclared variable: " << name);
		return vars.at(name);
	}

	std::vector<std::span<Token>> splitArgs(std::span<Token> tokens) {
	// trailing commas are permitted
		std::vector<std::span<Token>> result {};
		for (auto i=tokens.begin(), j=tokens.begin(); j!=tokens.end(); ++j) {
			if (j->type == TokenType::Comma) {
				result.push_back({i,j});
				i = std::next(j);
			} else if (std::next(j) == tokens.end()) {
				result.push_back({i,tokens.end()});
			}
		}
		return result;
	}

	std::set<ParamType> getParamTypes(std::span<Token> t) {
		std::set<ParamType> result {};
		using PT = ParamType;
		// registers, or condintions
		if (t.size()==1 && t[0].type == TokenType::Identifier) {
			for (const auto& [type, names] : ParamValTable)
				if (ranges::find(names, t[0].strValue) != names.end())
					result.insert(type);
		}
		// AF`
		else if (t.size()==2 && t[0].type == TokenType::Identifier) {
			if (t[0].strValue == "af" && t[1].type == TokenType::Tick)
				result.insert(PT::AF_p);
		}
		// indirection
		else if (t.front().type == TokenType::Paren0
		&&       t.back ().type == TokenType::Paren1) {
			// (IX), (DE), (C), etc...
			if (t[1].type == TokenType::Identifier) {
				if (t.size()==3) {
					for (const auto& [type, name] : ParamValTable_d)
						if (name[0] == t[1].strValue)
							result.insert(type);
				}
				// (IX+49), (IY), (IY-100), ...
				if (t[1].strValue == "ix") result.insert(PT::IX_d);
				if (t[1].strValue == "iy") result.insert(PT::IY_d);
			}
			// (0), ($8002), ...
			else if (holdsIntValue(t[1].type))
				result.insert({PT::n_d, PT::nn_d});
		}
		// numbers, labels, veriables
		if (holdsIntValue(t[0].type)
		&& !(t.front().type == TokenType::Paren0
		||   t.back ().type == TokenType::Paren1)
		&& !(t[0].type == TokenType::Identifier
		&&   isRegisterName(t[0].strValue))) {
			for (auto& [type, _ ] : NumberParamTypes)
				result.insert(type);
		}
		return result;
	}

	ParamVal getParamVal(Context& ctx, ParamType type, std::span<Token> t) {
		using PT = ParamType;
		// Index registers
		if (isAny(type, PT::IX_d, PT::IY_d)) {
			if (t.size() == 3) { return 0; }
			if (!isAny(t[2].type, TokenType::Plus, TokenType::Minus))
				PrintError("plus or minus expected in IX/IY");
			int sign = t[2].type == TokenType::Plus ? 1 : -1;

			integer n = parseExpression(ctx, t.subspan(3, t.size()-4));
			ParamVal d = sign * (signed)n;
			if (!(-128 <= d&&d <= 127)) { PrintError("out of bounds IX/IY offset"); }
			return d;
		}
		// Number parameters
		else if (isAny(type, PT::n   , PT::nn  , PT::d   , PT::b   ,
		/*                */ PT::n_d , PT::nn_d, PT::IMn , PT::RSTn)) {
			ParamVal n = parseExpression(ctx, t);
			if (!validNumberParam(type, n)) { PrintError("out of bounds number"); }
			return n;
		}
		// Short Jump
		else if (type == PT::e) {
			integer n = parseExpression(ctx, t);
			ParamVal e = n - ctx.progCounter;
			if (!validNumberParam(type, e)) { PrintError("out of bounds JR offset"); }
			return e;
		}

		// for group ParamTypes, the index in the vector is the ParamVal
		if (ParamValTable.contains(type)) {
			const auto& arr = ParamValTable.at(type);
			auto it = ranges::find(arr, t[0].strValue);
			if (it != arr.end())
				return std::distance(arr.begin(), it);
		}
		return {};
	}

	

	ExpressionContext::ExpressionContext(std::span<Token> tokens)
	: tokens   (tokens)
	, groups   (tokens.size())
	, evaluated(tokens.size()) {
		std::stack<std::size_t> parenStack;
		for (std::size_t i=0; i<tokens.size(); i++) {
			switch (tokens[i].type) {
				case TokenType::Paren0 : parenStack.push(i); break;
				case TokenType::Paren1 : groups[parenStack.top()] = i;
				/*                    */ groups[i] = parenStack.top();
				/*                    */ parenStack.pop(); break;
				default: groups[i] = i; break;
		} }
	}

	integer parseExpression(Context& ctx, std::span<Token> tokens) {
		auto expr = ExpressionContext(tokens);

		// parentheses / literals first
		iterateGroups(expr.groups, [&](auto i) {
			auto [i0, i1] = std::minmax(i, expr.groups[i]);
			if (i0 < i1) {
				std::span<Token> inParens = tokens.subspan(i0+1, i1-i0-1);
				integer parenValue = parseExpression(ctx, inParens);
				expr.evaluated[i0] = parenValue;
				expr.evaluated[i1] = parenValue;
			} else {
				expr.evaluated[i] = [&](Token& t) -> integer {
					using enum TokenType;
					if (t.type == Integer   ) { return t.intValue; }
					if (t.type == Dollar    ) { return ctx.progCounter; }
					if (t.type == Identifier) { return ctx.getVariable(t.strValue); }
					return {};
				}
				(tokens[i]);
			}
		});

		// Compile-time iteration through Order of Operations
		// I could not get std::apply to work, as passing the
		// elements of OP::Order as parameters de-constexpr's
		// them. If I'm revisiting this code in the future of
		// C++26, I'll replace with an 'expansion statement'.
		constexpr auto Size = std::tuple_size_v<decltype(OP::Order)>;
		[&]<std::size_t...I>(std::index_sequence<I...>) { (
			applyOperations
			<std::get<I>(OP::Order).first>
			(expr, std::get<I>(OP::Order).second)
			, ...
		); }
		(std::make_index_sequence<Size>{});

		return expr.evaluated[0];
	}



	template <OP::Assoc Dir, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext& expr, std::array<Fn,N> ops) {
		auto newGroups = expr.groups;

		auto matchOp = [&ops](auto token) -> std::optional<Fn> {
			auto result = ranges::find_if(ops,
				[&](auto op) { return op.token == token; }
			);
			if (result != ops.end()) { return *result; }
			return {};
		};

		auto neighbors = [&g=newGroups](auto i) {
			std::optional<std::size_t> prev, next;
			if (i   > 0       ) { prev = g[i-1]; }
			if (i+1 < g.size()) { next = g[i+1]; }
			return std::pair {prev,next};
		};

		if constexpr (std::is_same_v<Fn, OP::UnaryOpInfo>) {
			iterateGroups<Dir>(expr.groups, [&](auto i) {
				auto op = matchOp(expr.tokens[i].type);
				auto [prev,next] = neighbors(i);
				if (!op) { return; }

				auto& num = Dir==OP::Left ? prev : next;
				auto& adj = Dir==OP::Left ? next : prev;
				// Break if there's no expr to apply to
				if (!num) { return; }
				// Or if our op has two expr's like a binary-op
				if (adj && holdsIntValue(expr.tokens[*adj].type)) { return; }

				integer result = (*op)(expr.evaluated[*num]);
				expr.evaluated[*num] = result;
				expr.evaluated[i]    = result;
				newGroups[*num] = i;
				newGroups[i] = *num;
			} );
		} else if constexpr (std::is_same_v<Fn, OP::BinaryOpInfo>) {
			iterateGroups<Dir>(expr.groups, [&](auto i) {
				auto op = matchOp(expr.tokens[i].type);
				auto [prev,next] = neighbors(i);
				if (!op) { return; }
				// Break if either operands aren't expressions
				if (prev && !holdsIntValue(expr.tokens[*prev].type)) { return; }
				if (next && !holdsIntValue(expr.tokens[*next].type)) { return; }

				integer result = (*op)(expr.evaluated[*prev], expr.evaluated[*next]);
				expr.evaluated[*prev] = result;
				expr.evaluated[*next]   = result;
				newGroups[*prev] = *next;
				newGroups[*next] = *prev;
			} );
		}
		expr.groups = newGroups;
	};



	template <OP::Assoc Dir, typename Fn>
	void iterateGroups(const Grouping& groups, Fn f) {
		// Because indices are not permitted to be negative,
		// we let 0 represent one before the array beginning
		std::size_t begin = Dir==OP::Left ? 1u : groups.size();
		std::size_t end   = Dir==OP::Left ? groups.size()+1 : 0u;
		const int   step  = Dir==OP::Left ? 1 : -1;

		for (std::size_t i=begin; i!=end; ) {
			f(i-1);
			i = groups[i-1]+1 + step;
		}
	}
	// iterate left-first by default
	template <typename Fn>
	void iterateGroups(const Grouping& groups, Fn f) {
		iterateGroups<OP::Left>(groups,f);
	}
}