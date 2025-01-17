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
#include "../Object/Object.hpp"
#include <string>
#include <cstdint>

namespace cbor {
	class Encoder {
	private:
		Output* _out;
	
	public:
		Encoder(Output& out);
		
		~Encoder();
		
		auto write_bool(bool value) -> void;
		
		auto write_int(int32_t value) -> void;
		
		auto write_int(int64_t value) -> void;
		
		auto write_int(uint32_t value) -> void;
		
		auto write_int(uint64_t value) -> void;
		
		auto write_bytes(const uint8_t* data, uint32_t size) -> void;
		
		auto write_string(const char* data, uint32_t size) -> void;
		
		auto write_string(const std::string str) -> void;
		
		auto write_array(int size) -> void;
		
		auto write_map(int size) -> void;
		
		auto write_tag(const uint32_t tag) -> void;
		
		auto write_special(int special) -> void;
		
		auto write_null() -> void;
		
		auto write_undefined() -> void;
		
		auto write_object(PObject value) -> void;
	
	private:
		auto write_type_value(int major_type, uint32_t value) -> void;
		
		auto write_type_value(int major_type, uint64_t value) -> void;
	};
}
