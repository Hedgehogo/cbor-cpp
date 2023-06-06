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
	Decoder::Decoder(Input& in) :
		_in(&in), _state(DecoderState::Type), _minor_type(255) {
	}
	
	bool Decoder::has_bytes() {
		return _in->has_bytes(_current_length);
	}
	
	bool Decoder::has_bytes(int count) {
		return _in->has_bytes(count);
	}
	
	DecoderState Decoder::get_state() {
		return _state;
	}
	
	static PObject object_error(const std::string& error_msg) {
		return Object::from_error(error_msg);
	}
	
	static void put_decoded_value(DecodeData& decode_data, PObject value) {
		auto old_structures_stack_size = decode_data.structures_stack.size();
		if(decode_data.structures_stack.empty()) {
			if(decode_data.result)
				throw DecodeException("multiple cbor object when decoding");
			decode_data.result = value;
			
			if(value->object_type() == ObjectType::Array || value->object_type() == ObjectType::Map) {
				if(value->array_or_map_size > 0) {
					decode_data.structures_stack.push_back(value);
				}
			}
			return;
		}
		auto last = decode_data.structures_stack[old_structures_stack_size - 1];
		if(last->object_type() == ObjectType::Array) {
			auto& array_value = last->as<ObjectType::Array>();
			array_value.push_back(value);
			if(array_value.size() >= last->array_or_map_size) {
				// full, pop from structure
				decode_data.structures_stack.pop_back();
			}
		} else if(last->object_type() == ObjectType::Map) {
			if(decode_data.iter_in_map_key)
				decode_data.map_key_temp = value;
			else {
				if(decode_data.map_key_temp->object_type() != ObjectType::String) {
					throw DecodeException("invalid map key type");
				}
				const auto& key = decode_data.map_key_temp->as<ObjectType::String>();
				auto& map_value = last->as<ObjectType::Map>();
				map_value[key] = value;
				if(map_value.size() >= last->array_or_map_size) {
					// full, pop from structure
					decode_data.structures_stack.pop_back();
				}
			}
			decode_data.iter_in_map_key = !decode_data.iter_in_map_key;
		} else {
			throw DecodeException("invalid structure type");
		}
		
		if(value->object_type() == ObjectType::Array || value->object_type() == ObjectType::Map) {
			if(value->array_or_map_size > 0) {
				decode_data.structures_stack.push_back(value);
			}
		}
	}
	
	void Decoder::decode_type_p_int() {
		if(_minor_type < 24) {
			_state = DecoderState::PInt;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::PInt>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid integer type");
		}
	}
	
	void Decoder::decode_type_n_int() {
		if(_minor_type < 24) {
			_state = DecoderState::NInt;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::NInt>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid integer type");
		}
	}
	
	void Decoder::decode_type_bytes() {
		if(_minor_type < 24) {
			_state = DecoderState::BytesData;
			_current_length = _minor_type;
		} else if(!decode_type_count_length<DecoderState::BytesSize>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid bytes type");
		}
	}
	
	void Decoder::decode_type_string() {
		if(_minor_type < 24) {
			_state = DecoderState::StringData;
			_current_length = _minor_type;
		} else if(!decode_type_count_length<DecoderState::StringSize>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid string type");
		}
	}
	
	void Decoder::decode_type_array() {
		if(_minor_type < 24) {
			_state = DecoderState::Array;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::Array>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid array type");
		}
	}
	
	void Decoder::decode_type_map() {
		if(_minor_type < 24) {
			_state = DecoderState::Map;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::Map>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid array type");
		}
	}
	
	void Decoder::decode_type_tag() {
		if(_minor_type < 24) {
			_state = DecoderState::Tag;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::Tag>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid tag type");
		}
	}
	
	void Decoder::decode_type_special() {
		if(_minor_type < 24) {
			_state = DecoderState::Special;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::Special>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid special type");
		}
	}
	
	void Decoder::decode_type() {
		uint8_t type = _in->get_int8();
		uint8_t major_type = type >> 5;
		_minor_type = (uint8_t)(type & 0b00011111);
		
		switch(major_type) {
			case 0: // positive integer
				decode_type_p_int();
				break;
			case 1: // negative integer
				decode_type_n_int();
				break;
			case 2: // bytes
				decode_type_bytes();
				break;
			case 3: // string
				decode_type_string();
				break;
			case 4: // array
				decode_type_array();
				break;
			case 5: // map
				decode_type_map();
				break;
			case 6: // tag
				decode_type_tag();
				break;
			case 7: // special
				decode_type_special();
				break;
		}
	}
	
	PObject Decoder::decode_p_int() {
		_state = DecoderState::Type;
		if(_current_length != 8) {
			switch(_current_length) {
				case 0:
					return Object::from_int(_minor_type);
				case 1:
					return Object::from_int(_in->get_int8());
				case 2:
					return Object::from_int(_in->get_int16());
				case 4:
					unsigned int temp = _in->get_int32();
					if(temp <= INT_MAX) {
						return Object::from_int(temp);
					} else {
						return Object::from_extra_int(temp, true);
					}
			}
		} else {
			return Object::from_extra_int(_in->get_int64(), true);
		}
		_state = DecoderState::Error;
		throw DecodeException("incorrect length");
	}
	
	PObject Decoder::decode_n_int() {
		_state = DecoderState::Type;
		if(_current_length != 8) {
			switch(_current_length) {
				case 0:
					return Object::from_int(-1 - _minor_type);
				case 1:
					return Object::from_int(-(int)_in->get_int8() - 1);
				case 2:
					return Object::from_int(-(int)_in->get_int16() - 1);
				case 4:
					unsigned int temp = _in->get_int32();
					if(temp <= INT_MAX) {
						return Object::from_int(-(int)temp - 1);
					} else {
						return Object::from_extra_int(temp + 1, false);
					}
			}
		} else {
			return Object::from_extra_int(_in->get_int64() + 1, false);
		}
		_state = DecoderState::Error;
		throw DecodeException("incorrect length");
	}
	
	void Decoder::decode_bytes_size() {
		if(_current_length != 8) {
			_state = DecoderState::BytesData;
			switch(_current_length) {
				case 1:
					_current_length = _in->get_int8();
					break;
				case 2:
					_current_length = _in->get_int16();
					break;
				case 4:
					_current_length = _in->get_int32();
					break;
			}
		} else {
			_state = DecoderState::Error;
			throw DecodeException("extra long bytes");
		}
	}
	
	BytesValue Decoder::decode_bytes_data() {
		_state = DecoderState::Type;
		std::vector<char> data(_current_length);
		_in->get_bytes(data.data(), _current_length);
		return data;
	}
	
	void Decoder::decode_string_size() {
		if(_current_length != 8) {
			_state = DecoderState::StringData;
			switch(_current_length) {
				case 1:
					_current_length = _in->get_int8();
					break;
				case 2:
					_current_length = _in->get_int16();
					break;
				case 4:
					_current_length = _in->get_int32();
					break;
			}
		} else {
			_state = DecoderState::Error;
			throw DecodeException("extra long array");
		}
	}
	
	StringValue Decoder::decode_string_data() {
		_state = DecoderState::Type;
		std::vector<char> data(_current_length);
		_in->get_bytes(data.data(), _current_length);
		std::string str(data.data(), (size_t)_current_length);
		return str;
	}
	
	uint32_t Decoder::decode_array_size() {
		if(_current_length != 8) {
			_state = DecoderState::Type;
			switch(_current_length) {
				case 0:
					return _minor_type;
				case 1:
					return _in->get_int8();
				case 2:
					return _in->get_int16();
				case 4:
					return _in->get_int32();
			}
		}
		_state = DecoderState::Error;
		throw DecodeException("extra long array");
	}
	
	uint32_t Decoder::decode_map_size() {
		if(_current_length != 8) {
			_state = DecoderState::Type;
			switch(_current_length) {
				case 0:
					return _minor_type;
				case 1:
					return _in->get_int8();
				case 2:
					return _in->get_int16();
				case 4:
					return _in->get_int32();
			}
		}
		_state = DecoderState::Error;
		throw DecodeException("extra long map");
	}
	
	PObject Decoder::decode_tag() {
		_state = DecoderState::Type;
		switch(_current_length) {
			case 0:
				return Object::from_tag(_minor_type);
			case 1:
				return Object::from_tag(_in->get_int8());
			case 2:
				return Object::from_tag(_in->get_int16());
			case 4:
				return Object::from_tag(_in->get_int32());
			case 8:
				return Object::from_extra_tag(_in->get_int64());
		}
		_state = DecoderState::Error;
		throw DecodeException("incorrect length");
	}
	
	PObject Decoder::decode_special() {
		_state = DecoderState::Type;
		if(_minor_type < 20) {
			return Object::from_special(_minor_type);
		} else if(_minor_type == 20) {
			return Object::from_bool(false);
		} else if(_minor_type == 21) {
			return Object::from_bool(true);
		} else if(_minor_type == 22) {
			return Object::create_null();
		} else if(_minor_type == 23) {
			return Object::create_undefined();
		}
		switch(_current_length) {
			case 1:
				return Object::from_special(_in->get_int8());
			case 2:
				return Object::from_special(_in->get_int16());
			case 4:
				return Object::from_special(_in->get_int32());
			case 8:
				return Object::from_extra_special(_in->get_int64());
		}
		_state = DecoderState::Error;
		throw DecodeException("incorrect length");
	}
	
	PObject Decoder::run() {
		DecodeData decode_data{};
		
		while(true) {
			if(_state == DecoderState::Error) {
				break;
			} else if(_state == DecoderState::Type) {
				if(!_in->has_bytes(1))
					break;
				decode_type();
			} else {
				if(!_in->has_bytes(_current_length))
					break;
				switch(_state) {
					case DecoderState::PInt:
						put_decoded_value(decode_data, decode_p_int());
						break;
					case DecoderState::NInt:
						put_decoded_value(decode_data, decode_n_int());
						break;
					case DecoderState::BytesSize:
						decode_bytes_size();
						break;
					case DecoderState::BytesData:
						put_decoded_value(decode_data, Object::from_bytes(decode_bytes_data()));
						break;
					case DecoderState::StringSize:
						decode_string_size();
						break;
					case DecoderState::StringData:
						put_decoded_value(decode_data, Object::from_string(decode_string_data()));
						break;
					case DecoderState::Array:
						put_decoded_value(decode_data, Object::create_array(decode_array_size()));
						break;
					case DecoderState::Map:
						put_decoded_value(decode_data, Object::create_map(decode_map_size()));
						break;
					case DecoderState::Tag:
						put_decoded_value(decode_data, decode_tag());
						break;
					case DecoderState::Special:
						put_decoded_value(decode_data, decode_special());
						break;
					default:
						break;
				}
			}
		}
		if(!decode_data.result)
			throw DecodeException("cbor decoded nothing");
		if(!decode_data.structures_stack.empty())
			throw DecodeException("cbor decode fail with not finished structures");
		return decode_data.result;
	}
	
	Decoder::~Decoder() {
	}
}

