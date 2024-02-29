#include "Output.hpp"

#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>

namespace cbor {
	auto Output::bytes() const -> std::vector<unsigned char> {
		auto result{std::vector<unsigned char>(size())};
		memcpy(result.data(), data(), size());
		return result;
	}
}