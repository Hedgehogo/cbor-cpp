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

#include "Decoder.hpp"

#include <limits.h>

namespace cbor {
	Decoder::Decoder(Input& in) {
		_in = &in;
		_state = DecoderState::Type;
	}
	
	Decoder::~Decoder() {
	
	}
	
	static PObject cbor_object_error(const std::string& error_msg) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Error;
		result->value = error_msg;
		return result;
	}
	
	static void put_decoded_value(PObject& result, std::vector<PObject>& structures_stack, bool& iter_in_map_key, PObject& map_key_temp, PObject value) {
		auto old_structures_stack_size = structures_stack.size();
		if(structures_stack.empty()) {
			if(result)
				throw DecodeException("multiple cbor object when decoding");
			result = value;
			
			if(value->type == ObjectType::Array || value->type == ObjectType::Map) {
				if(value->array_or_map_size > 0) {
					structures_stack.push_back(value);
				}
			}
			return;
		}
		auto last = structures_stack[old_structures_stack_size - 1];
		if(last->type == ObjectType::Array) {
			auto& array_value = std::get<ArrayValue>(last->value);
			array_value.push_back(value);
			if(array_value.size() >= last->array_or_map_size) {
				// full, pop from structure
				structures_stack.pop_back();
			}
		} else if(last->type == ObjectType::Map) {
			if(iter_in_map_key)
				map_key_temp = value;
			else {
				if(map_key_temp->type != ObjectType::String) {
					throw DecodeException("invalid map key type");
				}
				const auto& key = std::get<StringValue>(map_key_temp->value);
				auto& map_value = std::get<MapValue>(last->value);
				map_value[key] = value;
				if(map_value.size() >= last->array_or_map_size) {
					// full, pop from structure
					structures_stack.pop_back();
				}
			}
			iter_in_map_key = !iter_in_map_key;
		} else {
			throw DecodeException("invalid structure type");
		}
		
		if(value->type == ObjectType::Array || value->type == ObjectType::Map) {
			if(value->array_or_map_size > 0) {
				structures_stack.push_back(value);
			}
		}
	}
	
	PObject Decoder::run() {
		PObject result;
		std::vector<PObject> structures_stack;
		bool iter_in_map_key = true;
		PObject map_key_temp;
		
		unsigned int temp;
		while(1) {
			if(_state == DecoderState::Type) {
				if(_in->has_bytes(1)) {
					unsigned char type = _in->get_byte();
					unsigned char majorType = type >> 5;
					unsigned char minorType = (unsigned char)(type & 31);
					
					switch(majorType) {
						case 0: // positive integer
							if(minorType < 24) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(minorType));
							} else if(minorType == 24) { // 1 byte
								_currentLength = 1;
								_state = DecoderState::PInt;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::PInt;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::PInt;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::PInt;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid integer type");
							}
							break;
						case 1: // negative integer
							if(minorType < 24) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(-1 - minorType));
							} else if(minorType == 24) { // 1 byte
								_currentLength = 1;
								_state = DecoderState::NInt;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::NInt;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::NInt;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::NInt;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid integer type");
							}
							break;
						case 2: // bytes
							if(minorType < 24) {
								_state = DecoderState::BytesData;
								_currentLength = minorType;
							} else if(minorType == 24) {
								_state = DecoderState::BytesSize;
								_currentLength = 1;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::BytesSize;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::BytesSize;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::BytesSize;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid bytes type");
							}
							break;
						case 3: // string
							if(minorType < 24) {
								_state = DecoderState::StringData;
								_currentLength = minorType;
							} else if(minorType == 24) {
								_state = DecoderState::StringSize;
								_currentLength = 1;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::StringSize;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::StringSize;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::StringSize;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid string type");
							}
							break;
						case 4: // array
							if(minorType < 24) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_array(minorType));
							} else if(minorType == 24) {
								_state = DecoderState::Array;
								_currentLength = 1;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::Array;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::Array;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::Array;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid array type");
							}
							break;
						case 5: // map
							if(minorType < 24) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_map(minorType));
							} else if(minorType == 24) {
								_state = DecoderState::Map;
								_currentLength = 1;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::Map;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::Map;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::Map;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid array type");
							}
							break;
						case 6: // tag
							if(minorType < 24) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_tag(minorType));
							} else if(minorType == 24) {
								_state = DecoderState::Tag;
								_currentLength = 1;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::Tag;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::Tag;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::Tag;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid tag type");
							}
							break;
						case 7: // special
							if(minorType < 20) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_special(minorType));
							} else if(minorType == 20) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_bool(false));
							} else if(minorType == 21) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_bool(true));
							} else if(minorType == 22) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_null());
							} else if(minorType == 23) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_undefined());
							} else if(minorType == 24) {
								_state = DecoderState::Special;
								_currentLength = 1;
							} else if(minorType == 25) { // 2 byte
								_currentLength = 2;
								_state = DecoderState::Special;
							} else if(minorType == 26) { // 4 byte
								_currentLength = 4;
								_state = DecoderState::Special;
							} else if(minorType == 27) { // 8 byte
								_currentLength = 8;
								_state = DecoderState::Special;
							} else {
								_state = DecoderState::Error;
								throw DecodeException("invalid special type");
							}
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::PInt) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(_in->get_byte()));
							_state = DecoderState::Type;
							break;
						case 2:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(_in->get_short()));
							_state = DecoderState::Type;
							break;
						case 4:
							temp = _in->get_int();
							if(temp <= INT_MAX) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(temp));
							} else {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_extra_integer(temp, true));
							}
							_state = DecoderState::Type;
							break;
						case 8:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_extra_integer(_in->get_long(), true));
							_state = DecoderState::Type;
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::NInt) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(-(int)_in->get_byte() - 1));
							_state = DecoderState::Type;
							break;
						case 2:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(-(int)_in->get_short() - 1));
							_state = DecoderState::Type;
							break;
						case 4:
							temp = _in->get_int();
							if(temp <= INT_MAX) {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_int(-(int)temp - 1));
							} else {
								put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_extra_integer(temp + 1, false));
							}
							_state = DecoderState::Type;
							break;
						case 8:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_extra_integer(_in->get_long() + 1, false));
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::BytesSize) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							_currentLength = _in->get_byte();
							_state = DecoderState::BytesData;
							break;
						case 2:
							_currentLength = _in->get_short();
							_state = DecoderState::BytesData;
							break;
						case 4:
							_currentLength = _in->get_int();
							_state = DecoderState::BytesData;
							break;
						case 8:
							_state = DecoderState::Error;
							throw DecodeException("extra long bytes");
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::BytesData) {
				if(_in->has_bytes(_currentLength)) {
					std::vector<char> data(_currentLength);
					_in->get_bytes(data.data(), _currentLength);
					_state = DecoderState::Type;
					put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_bytes(data));
				} else
					break;
			} else if(_state == DecoderState::StringSize) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							_currentLength = _in->get_byte();
							_state = DecoderState::StringData;
							break;
						case 2:
							_currentLength = _in->get_short();
							_state = DecoderState::StringData;
							break;
						case 4:
							_currentLength = _in->get_int();
							_state = DecoderState::StringData;
							break;
						case 8:
							_state = DecoderState::Error;
							throw DecodeException("extra long array");
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::StringData) {
				if(_in->has_bytes(_currentLength)) {
					std::vector<char> data(_currentLength);
					_in->get_bytes(data.data(), _currentLength);
					_state = DecoderState::Type;
					std::string str(data.data(), (size_t)_currentLength);
					put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_string(str));
				} else
					break;
			} else if(_state == DecoderState::Array) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_array(_in->get_byte()));
							_state = DecoderState::Type;
							break;
						case 2:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_array(_in->get_short()));
							_state = DecoderState::Type;
							break;
						case 4:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_array(_in->get_int()));
							_state = DecoderState::Type;
							break;
						case 8:
							_state = DecoderState::Error;
							throw DecodeException("extra long array");
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::Map) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_map(_in->get_byte()));
							_state = DecoderState::Type;
							break;
						case 2:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_map(_currentLength = _in->get_short()));
							_state = DecoderState::Type;
							break;
						case 4:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::create_map(_in->get_int()));
							_state = DecoderState::Type;
							break;
						case 8:
							_state = DecoderState::Error;
							throw DecodeException("extra long map");
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::Tag) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_tag(_in->get_byte()));
							_state = DecoderState::Type;
							break;
						case 2:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_tag(_in->get_short()));
							_state = DecoderState::Type;
							break;
						case 4:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_tag(_in->get_int()));
							_state = DecoderState::Type;
							break;
						case 8:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_extra_tag(_in->get_long()));
							_state = DecoderState::Type;
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::Special) {
				if(_in->has_bytes(_currentLength)) {
					switch(_currentLength) {
						case 1:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_special(_in->get_byte()));
							_state = DecoderState::Type;
							break;
						case 2:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_special(_in->get_short()));
							_state = DecoderState::Type;
							break;
						case 4:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_special(_in->get_int()));
							_state = DecoderState::Type;
							break;
						case 8:
							put_decoded_value(result, structures_stack, iter_in_map_key, map_key_temp, Object::from_extra_special(_in->get_long()));
							_state = DecoderState::Type;
							break;
					}
				} else
					break;
			} else if(_state == DecoderState::Error) {
				break;
			} else {
				throw DecodeException("UNKNOWN STATE");
			}
		}
		if(!result)
			throw DecodeException("cbor decoded nothing");
		if(!structures_stack.empty())
			throw DecodeException("cbor decode fail with not finished structures");
		return result;
	}
}

