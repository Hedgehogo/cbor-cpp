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

#include "../Output/Output.hpp"

namespace cbor {
	class OutputStatic : public Output {
	private:
		unsigned char* _buffer;
		unsigned int _capacity;
		unsigned int _offset;
	
	public:
		OutputStatic(unsigned int capacity);
		
		~OutputStatic();
		
		virtual unsigned char* getData() const;
		
		virtual unsigned int getSize() const;
		
		virtual void put_byte(unsigned char value);
		
		virtual void put_bytes(const unsigned char* data, int size);
	};
}

