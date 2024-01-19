#pragma once
#include <iostream>
#include <array>

enum struct Word { LSB, MSB };


// iteger type
template <std::integral T>
constexpr auto Bytes(T n) {
	return std::array<std::byte,1> { std::byte(n) };
}

template <Word w>
constexpr auto Bytes(auto n) {
	if (w == Word::LSB)
		return std::array<std::byte,2> { std::byte(n), std::byte(n>>8) };
	else if (w == Word::MSB)
		return std::array<std::byte,2> { std::byte(n>>8), std::byte(n) };
}

// string of size M
template <std::size_t M, std::size_t N>
constexpr auto Bytes(const char (&str)[N]) {
	std::array<std::byte,M> result = {};
	for (std::size_t i=0; i<M && str[i]!='\0'; i++)
		result[i] = std::byte(str[i]);
	return result;
}

// TODO: unambiguate with upper function when sizeof(str) == M
//	// string of constant size
//	template <std::size_t N>
//	constexpr auto Bytes(const char (&str)[N]) {
//		return Bytes<N-1,N>(str);
//	}




template<typename T>
concept ByteContainer = std::is_same_v<typename T::value_type,std::byte>;

// output single byte
std::ostream& operator<<(std::ostream& os, const std::byte& b) {
	return os.put((char)b);
}

// output array/vector of bytes
template <ByteContainer T>
std::ostream& operator<<(std::ostream& os, const T& bytes) {
	for (std::byte b : bytes) { os << b; }
	return os;
}