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

#include <cstdint>

namespace cbor {
	class Input {
	private:
		uint8_t* _data;
		int _size;
		int _offset;
	
	public:
		Input(void* data, int size);
		
		auto has_bytes(int count) -> bool;
		
		auto is_empty() -> bool;
		
		auto get_int8() -> uint8_t;
		
		auto get_int16() -> uint16_t;
		
		auto get_int32() -> uint32_t;
		
		auto get_int64() -> uint64_t;
		
		auto get_bytes(void* to, int count) -> void;
		
		~Input();
	};
}

