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
	void OutputDynamic::init(unsigned int inital_capacity) {
		this->_capacity = inital_capacity;
		this->_buffer = new unsigned char[inital_capacity];
		this->_offset = 0;
	}
	
	OutputDynamic::OutputDynamic() {
		init(256);
	}
	
	OutputDynamic::OutputDynamic(unsigned int inital_capacity) {
		init(inital_capacity);
	}
	
	OutputDynamic::~OutputDynamic() {
		delete _buffer;
	}
	
	unsigned char* OutputDynamic::data() const {
		return _buffer;
	}
	
	unsigned int OutputDynamic::size() const {
		return _offset;
	}
	
	void OutputDynamic::put_byte(unsigned char value) {
		if(_offset < _capacity) {
			_buffer[_offset++] = value;
		} else {
			_capacity *= 2;
			_buffer = (unsigned char*)realloc(_buffer, _capacity);
			_buffer[_offset++] = value;
		}
	}
	
	void OutputDynamic::put_bytes(const unsigned char* data, int size) {
		while(_offset + size > _capacity) {
			_capacity *= 2;
			_buffer = (unsigned char*)realloc(_buffer, _capacity);
		}
		
		memcpy(_buffer + _offset, data, size);
		_offset += size;
	}
}
