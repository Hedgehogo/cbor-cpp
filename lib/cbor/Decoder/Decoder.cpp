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
	
	void Decoder::set_type_state() {
		_state = DecoderState::Type;
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
		} else if(!decode_type_count_length<DecoderState::PInt, DecoderState::ExtraPInt>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid integer type");
		}
	}
	
	void Decoder::decode_type_n_int() {
		if(_minor_type < 24) {
			_state = DecoderState::NInt;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::NInt, DecoderState::ExtraNInt>(_minor_type)) {
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
		} else if(!decode_type_count_length<DecoderState::Tag, DecoderState::ExtraTag>(_minor_type)) {
			_state = DecoderState::Error;
			throw DecodeException("invalid tag type");
		}
	}
	
	void Decoder::decode_type_special() {
		if(_minor_type < 20) {
			_state = DecoderState::Special;
			_current_length = 0;
		} else if(_minor_type == 20) {
			_state = DecoderState::BoolFalse;
			_current_length = 0;
		} else if(_minor_type == 21) {
			_state = DecoderState::BoolTrue;
			_current_length = 0;
		} else if(_minor_type == 22) {
			_state = DecoderState::Null;
			_current_length = 0;
		} else if(_minor_type == 23) {
			_state = DecoderState::Undefined;
			_current_length = 0;
		} else if(!decode_type_count_length<DecoderState::Special, DecoderState::ExtraSpecial>(_minor_type)) {
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
	
	IntValue Decoder::decode_p_int() {
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
		_state = DecoderState::Error;
		throw DecodeException("incorrect length");
	}
	
	IntValue Decoder::decode_n_int() {
		_state = DecoderState::Type;
		switch(_current_length) {
			case 0:
				return -1 - _minor_type;
			case 1:
				return -(int64_t)_in->get_int8() - 1;
			case 2:
				return -(int64_t)_in->get_int16() - 1;
			case 4:
				return -(int64_t)_in->get_int32() - 1;
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
	
	TagValue Decoder::decode_tag() {
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
		_state = DecoderState::Error;
		throw DecodeException("extra long tag");
	}
	
	SpecialValue Decoder::decode_special() {
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
		_state = DecoderState::Error;
		throw DecodeException("extra long special");
	}
	
	ExtraIntValue Decoder::decode_extra_p_int() {
		_state = DecoderState::Type;
		return {true, _in->get_int64()};
	}
	
	ExtraIntValue Decoder::decode_extra_n_int() {
		_state = DecoderState::Type;
		return {false, _in->get_int64() + 1};
	}
	
	ExtraTagValue Decoder::decode_extra_tag() {
		_state = DecoderState::Type;
		return _in->get_int64();
	}
	
	ExtraSpecialValue Decoder::decode_extra_special() {
		_state = DecoderState::Type;
		return _in->get_int64();
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
						put_decoded_value(decode_data, Object::from_int(decode_p_int()));
						break;
					case DecoderState::NInt:
						put_decoded_value(decode_data, Object::from_int(decode_n_int()));
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
						put_decoded_value(decode_data, Object::from_tag(decode_tag()));
						break;
					case DecoderState::Special:
						put_decoded_value(decode_data, Object::from_special(decode_special()));
						break;
					case DecoderState::BoolFalse:
						_state = DecoderState::Type;
						put_decoded_value(decode_data, Object::from_bool(false));
						break;
					case DecoderState::BoolTrue:
						_state = DecoderState::Type;
						put_decoded_value(decode_data, Object::from_bool(true));
						break;
					case DecoderState::Null:
						_state = DecoderState::Type;
						put_decoded_value(decode_data, Object::create_null());
						break;
					case DecoderState::Undefined:
						_state = DecoderState::Type;
						put_decoded_value(decode_data, Object::create_undefined());
						break;
					case DecoderState::ExtraPInt:
						put_decoded_value(decode_data, Object::from_extra_int(decode_extra_p_int()));
						break;
					case DecoderState::ExtraNInt:
						put_decoded_value(decode_data, Object::from_extra_int(decode_extra_n_int()));
						break;
					case DecoderState::ExtraTag:
						put_decoded_value(decode_data, Object::from_extra_tag(decode_extra_tag()));
						break;
					case DecoderState::ExtraSpecial:
						put_decoded_value(decode_data, Object::from_extra_special(decode_extra_special()));
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

