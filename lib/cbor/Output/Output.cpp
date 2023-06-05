#include "Output.hpp"

#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>

namespace cbor {
	std::vector<unsigned char> Output::bytes() const {
		std::vector<unsigned char> result(size());
		memcpy(result.data(), data(), size());
		return result;
	}
}