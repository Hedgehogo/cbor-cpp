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

#include "OutputDynamic.hpp"

#include <string.h>
#include <stdlib.h>

namespace cbor {
	OutputDynamic::OutputDynamic(unsigned int inital_capacity) {
		init(inital_capacity);
	}
	
	OutputDynamic::OutputDynamic() {
		init(256);
	}
	
	auto OutputDynamic::init(unsigned int inital_capacity) -> void {
		this->_capacity = inital_capacity;
		this->_buffer = new unsigned char[inital_capacity];
		this->_offset = 0;
	}
	
	auto OutputDynamic::data() const -> unsigned char* {
		return _buffer;
	}
	
	auto OutputDynamic::size() const -> unsigned int {
		return _offset;
	}
	
	auto OutputDynamic::put_byte(unsigned char value) -> void {
		if(_offset < _capacity) {
			_buffer[_offset++] = value;
		} else {
			_capacity *= 2;
			_buffer = (unsigned char*)realloc(_buffer, _capacity);
			_buffer[_offset++] = value;
		}
	}
	
	auto OutputDynamic::put_bytes(unsigned char const* data, int size) -> void {
		while(_offset + size > _capacity) {
			_capacity *= 2;
			_buffer = (unsigned char*)realloc(_buffer, _capacity);
		}
		
		memcpy(_buffer + _offset, data, size);
		_offset += size;
	}
	
	OutputDynamic::~OutputDynamic() {
		delete _buffer;
	}
}
