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
	public:
		Decoder(Input& in);
		
		auto has_bytes() -> bool;
		
		auto has_bytes(int count) -> bool;
		
		auto set_type_state() -> void;
		
		auto get_state() -> DecoderState;
		
		auto decode_type_p_int() -> void;
		
		auto decode_type_n_int() -> void;
		
		auto decode_type_bytes() -> void;
		
		auto decode_type_string() -> void;
		
		auto decode_type_array() -> void;
		
		auto decode_type_map() -> void;
		
		auto decode_type_tag() -> void;
		
		auto decode_type_special() -> void;
		
		auto decode_type() -> void;
		
		auto decode_p_int() -> IntValue;
		
		auto decode_n_int() -> IntValue;
		
		auto decode_bytes_size() -> void;
		
		auto decode_bytes_data() -> BytesValue;
		
		auto decode_string_size() -> void;
		
		auto decode_string_data() -> StringValue;
		
		auto decode_array_size() -> uint32_t;
		
		auto decode_map_size() -> uint32_t;
		
		auto decode_tag() -> TagValue;
		
		auto decode_special() -> SpecialValue;
		
		auto decode_extra_p_int() -> ExtraIntValue;
		
		auto decode_extra_n_int() -> ExtraIntValue;
		
		auto decode_extra_tag() -> ExtraTagValue;
		
		auto decode_extra_special() -> ExtraSpecialValue;
		
		auto run() -> PObject;
		
		~Decoder();
	
	private:
		template<DecoderState State, DecoderState LastState = State>
		auto decode_type_count_length(unsigned char minor_type) -> bool;
		
		Input* _in;
		DecoderState _state;
		int _current_length;
		uint8_t _minor_type;
	};
}

#include "Decoder.inl"
