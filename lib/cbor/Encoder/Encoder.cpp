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

#include "Encoder.hpp"

namespace cbor {
	Encoder::Encoder(Output& out) {
		_out = &out;
	}
	
	Encoder::~Encoder() {
	}
	
	void Encoder::write_type_value(int major_type, uint32_t value) {
		major_type <<= 5;
		if(value < 24) {
			_out->put_byte((uint8_t)(major_type | value));
		} else if(value < 256) {
			_out->put_byte((uint8_t)(major_type | 24));
			_out->put_byte((uint8_t)value);
		} else if(value < 65536) {
			_out->put_byte((uint8_t)(major_type | 25));
			_out->put_byte((uint8_t)(value >> 8));
			_out->put_byte((uint8_t)value);
		} else {
			_out->put_byte((uint8_t)(major_type | 26));
			_out->put_byte((uint8_t)(value >> 24));
			_out->put_byte((uint8_t)(value >> 16));
			_out->put_byte((uint8_t)(value >> 8));
			_out->put_byte((uint8_t)value);
		}
	}
	
	void Encoder::write_type_value(int major_type, uint64_t value) {
		major_type <<= 5;
		if(value < 24ULL) {
			_out->put_byte((uint8_t)(major_type | value));
		} else if(value < 256ULL) {
			_out->put_byte((uint8_t)(major_type | 24));
			_out->put_byte((uint8_t)value);
		} else if(value < 65536ULL) {
			_out->put_byte((uint8_t)(major_type | 25));
			_out->put_byte((uint8_t)(value >> 8));
			_out->put_byte((uint8_t)value);
		} else if(value < 4294967296ULL) {
			_out->put_byte((uint8_t)(major_type | 26));
			_out->put_byte((uint8_t)(value >> 24));
			_out->put_byte((uint8_t)(value >> 16));
			_out->put_byte((uint8_t)(value >> 8));
			_out->put_byte((uint8_t)value);
		} else {
			_out->put_byte((uint8_t)(major_type | 27));
			_out->put_byte((uint8_t)(value >> 56));
			_out->put_byte((uint8_t)(value >> 48));
			_out->put_byte((uint8_t)(value >> 40));
			_out->put_byte((uint8_t)(value >> 32));
			_out->put_byte((uint8_t)(value >> 24));
			_out->put_byte((uint8_t)(value >> 16));
			_out->put_byte((uint8_t)(value >> 8));
			_out->put_byte((uint8_t)value);
		}
	}
	
	void Encoder::write_int(uint32_t value) {
		write_type_value(0, value);
	}
	
	void Encoder::write_int(uint64_t value) {
		write_type_value(0, value);
	}
	
	void Encoder::write_int(int64_t value) {
		if(value < 0) {
			write_type_value(1, (uint64_t)-(value + 1));
		} else {
			write_type_value(0, (uint64_t)value);
		}
	}
	
	void Encoder::write_int(int32_t value) {
		if(value < 0) {
			write_type_value(1, (uint32_t)-(value + 1));
		} else {
			write_type_value(0, (uint32_t)value);
		}
	}
	
	void Encoder::write_bytes(const uint8_t* data, uint32_t size) {
		write_type_value(2, size);
		_out->put_bytes(data, size);
	}
	
	void Encoder::write_string(const char* data, uint32_t size) {
		write_type_value(3, size);
		_out->put_bytes((const uint8_t*)data, size);
	}
	
	void Encoder::write_string(const std::string str) {
		write_type_value(3, (uint32_t)str.size());
		_out->put_bytes((const uint8_t*)str.c_str(), (int)str.size());
	}
	
	
	void Encoder::write_array(int size) {
		write_type_value(4, (uint32_t)size);
	}
	
	void Encoder::write_map(int size) {
		write_type_value(5, (uint32_t)size);
	}
	
	void Encoder::write_tag(const uint32_t tag) {
		write_type_value(6, tag);
	}
	
	void Encoder::write_special(int special) {
		write_type_value(7, (uint32_t)special);
	}
	
	void Encoder::write_bool(bool value) {
		if(value) {
			_out->put_byte((uint8_t)0xf5);
		} else {
			_out->put_byte((uint8_t)0xf4);
		}
	}
	
	void Encoder::write_null() {
		_out->put_byte((uint8_t)0xf6);
	}
	
	void Encoder::write_undefined() {
		_out->put_byte((uint8_t)0xf7);
	}
	
	void Encoder::write_object(PObject value) {
		if(!value)
			return;
		switch(value->object_type()) {
			case ObjectType::Null:
				write_null();
				return;
			case ObjectType::Undefined:
				write_undefined();
				return;
			case ObjectType::Bool:
				write_bool(value->as_bool());
				return;
			case ObjectType::Int:
				write_int(value->as_int());
				return;
			case ObjectType::ExtraInt:
				write_int(value->as<ObjectType::ExtraInt>().second);
				return;
			case ObjectType::String:
				write_string(value->as_string());
				return;
			case ObjectType::Bytes: {
				const auto& bytes = value->as_bytes();
				write_bytes((const uint8_t*)bytes.data(), bytes.size());
				return;
			}
			case ObjectType::Tag:
				write_tag(value->as_tag());
				return;
			case ObjectType::ExtraTag:
				write_tag(value->as<ObjectType::ExtraTag>());
				return;
			case ObjectType::Special:
				write_special(value->as_special());
				return;
			case ObjectType::ExtraSpecial:
				write_special(value->as<ObjectType::ExtraSpecial>());
				return;
			case ObjectType::Array: {
				const auto& array_value = value->as_array();
				write_array(array_value.size());
				for(const auto& item: array_value) {
					write_object(item);
				}
				return;
			}
			case ObjectType::Map: {
				const auto& map_value = value->as_map();
				write_map(map_value.size());
				for(const auto& p: map_value) {
					write_string(p.first);
					write_object(p.second);
				}
				return;
			}
			case ObjectType::Error: {
				throw EncodeException("invalid cbor object type");
			}
		}
	}
}
