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

#include "Input.hpp"

#include <stdlib.h>
#include <string.h>

namespace cbor {
	Input::Input(void* data, int size) :
		_data((uint8_t*)data), _size(size), _offset(0) {
	}
	
	auto Input::has_bytes(int count) -> bool {
		return _size - _offset >= count;
	}
	
	auto Input::get_int8() -> uint8_t {
		return _data[_offset++];
	}
	
	auto Input::get_int16() -> uint16_t {
		uint16_t value = ((uint16_t)_data[_offset] << 8) | ((uint16_t)_data[_offset + 1]);
		_offset += 2;
		return value;
	}
	
	auto Input::get_int32() -> uint32_t {
		uint32_t value =
			((uint32_t)_data[_offset] << 24) |
			((uint32_t)_data[_offset + 1] << 16) |
			((uint32_t)_data[_offset + 2] << 8) |
			((uint32_t)_data[_offset + 3]);
		_offset += 4;
		return value;
	}
	
	auto Input::get_int64() -> uint64_t {
		uint64_t value =
			((uint64_t)_data[_offset] << 56) |
			((uint64_t)_data[_offset + 1] << 48) | ((uint64_t)_data[_offset + 2] << 40) |
			((uint64_t)_data[_offset + 3] << 32) | ((uint64_t)_data[_offset + 4] << 24) |
			((uint64_t)_data[_offset + 5] << 16) | ((uint64_t)_data[_offset + 6] << 8) |
			((uint64_t)_data[_offset + 7]);
		_offset += 8;
		return value;
	}
	
	auto Input::get_bytes(void* to, int count) -> void {
		memcpy(to, _data + _offset, count);
		_offset += count;
	}
	
	Input::~Input() {
	}
	
	auto Input::is_empty() -> bool {
		return _size == _offset;
	}
}
