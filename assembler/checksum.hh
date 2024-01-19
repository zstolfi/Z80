#pragma once
#include "bytes.hh"

class Checksum {
private:
	std::ostream* os;
public:
	int value=0;

	friend Checksum& operator<<(std::ostream& os, Checksum& cs) {
		cs.os = &os;
		return cs;
	}

	template <ByteContainer T>
	friend Checksum& operator<<(Checksum& cs, const T& bytes) {
		for (std::byte b : bytes) {
			cs.value += (int)b;
		}
		(*cs.os) << bytes;
		return cs;
	}
};