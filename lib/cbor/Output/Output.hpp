/*
   Copyright 2014-2015 Stanislav Ovsyannikov

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

	   Unless required by applicable law or agreed to in writing, software
	   distributed under the License is distributed on an "AS IS" BASIS,
	   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	   See the License for the specific language governing permissions and
	   limitations under the License.
*/

#pragma once

#include <vector>
#include <string>

namespace cbor {
	class Output {
	public:
		virtual auto data() const -> unsigned char* = 0;
		
		virtual auto size() const -> unsigned int = 0;
		
		virtual auto bytes() const -> std::vector<unsigned char>;
		
		virtual auto put_byte(unsigned char value) -> void = 0;
		
		virtual auto put_bytes(const unsigned char* data, int size) -> void = 0;
	};
}

