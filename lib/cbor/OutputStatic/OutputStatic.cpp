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

#include "OutputStatic.hpp"
#include "../Exceptions/Exceptions.hpp"

#include <string.h>

namespace cbor {
	OutputStatic::OutputStatic(unsigned int capacity) {
		this->_capacity = capacity;
		this->_buffer = new unsigned char[capacity];
		this->_offset = 0;
	}
	
	auto OutputStatic::data() const -> unsigned char* {
		return _buffer;
	}
	
	auto OutputStatic::size() const -> unsigned int {
		return _offset;
	}
	
	auto OutputStatic::put_byte(unsigned char value) -> void {
		if(_offset < _capacity) {
			_buffer[_offset++] = value;
		} else {
			throw OutputException("buffer overflow error");
		}
	}
	
	auto OutputStatic::put_bytes(unsigned char const* data, int size) -> void {
		if(_offset + size - 1 < _capacity) {
			memcpy(_buffer + _offset, data, size);
			_offset += size;
		} else {
			throw OutputException("buffer overflow error");
		}
	}
	
	OutputStatic::~OutputStatic() {
		delete _buffer;
	}
}
