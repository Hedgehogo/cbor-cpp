#pragma once

#include <variant>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include "../Exceptions/Exceptions.hpp"

namespace cbor {
	enum class ObjectType {
		Bool,
		Int,
		Bytes,
		String,
		Array,
		Map,
		Tag,
		Special,
		Undefined,
		Null,
		Error,
		ExtraInt,
		ExtraTag,
		ExtraSpecial,
	};
	
	struct Object;
	using PObject = std::shared_ptr<Object>;
	using BoolValue = bool;
	using IntValue = int64_t;
	using TagValue = uint32_t;
	using BytesValue = std::vector<char>;
	using ArrayValue = std::vector<PObject>;
	using MapValue = std::map<std::string, PObject, std::less<std::string> >;
	using StringValue = std::string;
	using ErrorValue = std::string;
	using ExtraIntValue = uint64_t;
	using SpecialValue = uint32_t;
	using ExtraSpecialValue = uint64_t;
	using ObjectValue = std::variant<BoolValue, IntValue, ExtraIntValue, TagValue, StringValue, BytesValue, ArrayValue, MapValue>;
	
	struct Object {
		ObjectType type;
		ObjectValue value;
		uint32_t array_or_map_size = 0;
		bool is_positive_extra = false;
		
		template<typename T>
		const T& as() const {
			return std::get<T>(value);
		}
		
		inline bool is_null() const {
			return ObjectType::Null == type;
		}
		
		inline bool is_undefined() const {
			return ObjectType::Undefined == type;
		}
		
		inline bool is_int() const {
			return ObjectType::Int == type;
		}
		
		inline bool is_string() const {
			return ObjectType::String == type;
		}
		
		inline bool is_bytes() const {
			return ObjectType::Bytes == type;
		}
		
		inline bool is_bool() const {
			return ObjectType::Bool == type;
		}
		
		inline bool is_array() const {
			return ObjectType::Array == type;
		}
		
		inline bool is_map() const {
			return ObjectType::Map == type;
		}
		
		inline bool is_tag() const {
			return ObjectType::Tag == type;
		}
		
		inline bool is_special() const {
			return ObjectType::Special == type;
		}
		
		inline ObjectType object_type() const {
			return type;
		}
		
		inline const BoolValue& as_bool() const {
			return as<BoolValue>();
		}
		
		inline const IntValue& as_int() const {
			return as<IntValue>();
		}
		
		inline const TagValue& as_tag() const {
			return as<TagValue>();
		}
		
		inline const SpecialValue& as_special() const {
			return as<SpecialValue>();
		}
		
		inline const BytesValue& as_bytes() const {
			return as<BytesValue>();
		}
		
		inline const ArrayValue& as_array() const {
			return as<ArrayValue>();
		}
		
		inline const MapValue& as_map() const {
			return as<MapValue>();
		}
		
		inline const StringValue& as_string() const {
			return as<StringValue>();
		}
		
		static PObject from_int(IntValue value);
		
		static PObject from_bool(BoolValue value);
		
		static PObject from_bytes(const BytesValue& value);
		
		static PObject from_string(const std::string& value);
		
		static PObject create_array(size_t size);
		
		static PObject create_map(size_t size);
		
		static PObject from_tag(TagValue value);
		
		static PObject create_undefined();
		
		static PObject create_null();
		
		static PObject from_special(uint32_t value);
		
		static PObject from_extra_integer(uint64_t value, bool sign);
		
		static PObject from_extra_tag(uint64_t value);
		
		static PObject from_extra_special(uint64_t value);
	};
}
