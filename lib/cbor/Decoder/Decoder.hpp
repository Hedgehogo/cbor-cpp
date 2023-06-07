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

#include "../Input/Input.hpp"
#include "../Object/Object.hpp"

namespace cbor {
	enum class DecoderState {
		Type,
		PInt,
		NInt,
		BytesSize,
		BytesData,
		StringSize,
		StringData,
		Array,
		Map,
		Tag,
		Special,
		BoolFalse,
		BoolTrue,
		Null,
		Undefined,
		Error,
		ExtraPInt,
		ExtraNInt,
		ExtraTag,
		ExtraSpecial,
	};
	
	struct DecodeData {
		PObject result;
		std::vector<PObject> structures_stack;
		bool iter_in_map_key = true;
		PObject map_key_temp;
	};
	
	class Decoder {
	private:
		Input* _in;
		DecoderState _state;
		int _current_length;
		uint8_t _minor_type;
		
		template<DecoderState State, DecoderState LastState = State>
		bool decode_type_count_length(unsigned char minor_type) {
			if(minor_type == 24) { // 1 byte
				_current_length = 1;
				_state = State;
			} else if(minor_type == 25) { // 2 byte
				_current_length = 2;
				_state = State;
			} else if(minor_type == 26) { // 4 byte
				_current_length = 4;
				_state = State;
			} else if(minor_type == 27) { // 8 byte
				_current_length = 8;
				_state = LastState;
			} else {
				return false;
			}
			return true;
		}
	
	public:
		Decoder(Input& in);
		
		bool has_bytes();
		
		bool has_bytes(int count);
		
		void set_type_state();
		
		DecoderState get_state();
		
		void decode_type_p_int();
		
		void decode_type_n_int();
		
		void decode_type_bytes();
		
		void decode_type_string();
		
		void decode_type_array();
		
		void decode_type_map();
		
		void decode_type_tag();
		
		void decode_type_special();
		
		void decode_type();
		
		IntValue decode_p_int();
		
		IntValue decode_n_int();
		
		void decode_bytes_size();
		
		BytesValue decode_bytes_data();
		
		void decode_string_size();
		
		StringValue decode_string_data();
		
		uint32_t decode_array_size();
		
		uint32_t decode_map_size();
		
		TagValue decode_tag();
		
		SpecialValue decode_special();
		
		ExtraIntValue decode_extra_p_int();
		
		ExtraIntValue decode_extra_n_int();
		
		ExtraTagValue decode_extra_tag();
		
		ExtraSpecialValue decode_extra_special();
		
		PObject run();
		
		~Decoder();
	};
}
