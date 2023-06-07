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
	using BytesValue = std::vector<char>;
	using StringValue = std::string;
	using ArrayValue = std::vector<PObject>;
	using MapValue = std::map<std::string, PObject, std::less<std::string> >;
	using TagValue = uint32_t;
	using SpecialValue = uint32_t;
	using UndefinedValue = std::monostate;
	using NullValue = std::monostate;
	using ErrorValue = std::string;
	using ExtraIntValue = std::pair<bool, uint64_t>;
	using ExtraTagValue = uint64_t;
	using ExtraSpecialValue = uint64_t;
	
	using ObjectValue = std::variant<
		BoolValue,
		IntValue,
		BytesValue,
		StringValue,
		ArrayValue,
		MapValue,
		TagValue,
		SpecialValue,
		UndefinedValue,
		NullValue,
		ErrorValue,
		ExtraIntValue,
		ExtraTagValue,
		ExtraSpecialValue
	>;
	
	template<ObjectType Type>
	using ObjectValueType = std::variant_alternative_t<static_cast<size_t>(Type), ObjectValue>;
	
	struct Object {
		ObjectValue value;
		uint32_t array_or_map_size = 0;
		
		template<ObjectType Type>
		void set(const ObjectValueType<Type>& new_value) {
			value.emplace<static_cast<size_t>(Type)>(new_value);
		}
		
		template<ObjectType Type>
		ObjectValueType<Type>& as() {
			return std::get<static_cast<size_t>(Type)>(value);
		}
		
		template<ObjectType Type>
		const ObjectValueType<Type>& as() const {
			return std::get<static_cast<size_t>(Type)>(value);
		}
		
		template<ObjectType Type>
		inline bool is() const {
			return Type == static_cast<ObjectType>(value.index());
		}
		
		inline bool is_null() const {
			return is<ObjectType::Null>();
		}
		
		inline bool is_undefined() const {
			return is<ObjectType::Undefined>();
		}
		
		inline bool is_int() const {
			return is<ObjectType::Int>();
		}
		
		inline bool is_string() const {
			return is<ObjectType::String>();
		}
		
		inline bool is_bytes() const {
			return is<ObjectType::Bytes>();
		}
		
		inline bool is_bool() const {
			return is<ObjectType::Bool>();
		}
		
		inline bool is_array() const {
			return is<ObjectType::Array>();
		}
		
		inline bool is_map() const {
			return is<ObjectType::Map>();
		}
		
		inline bool is_tag() const {
			return is<ObjectType::Tag>();
		}
		
		inline bool is_special() const {
			return is<ObjectType::Special>();
		}
		
		inline ObjectType object_type() const {
			return static_cast<ObjectType>(value.index());
		}
		
		inline const BoolValue& as_bool() const {
			return as<ObjectType::Bool>();
		}
		
		inline const IntValue& as_int() const {
			return as<ObjectType::Int>();
		}
		
		inline const BytesValue& as_bytes() const {
			return as<ObjectType::Bytes>();
		}
		
		inline const StringValue& as_string() const {
			return as<ObjectType::String>();
		}
		
		inline const ArrayValue& as_array() const {
			return as<ObjectType::Array>();
		}
		
		inline const MapValue& as_map() const {
			return as<ObjectType::Map>();
		}
		
		inline const TagValue& as_tag() const {
			return as<ObjectType::Tag>();
		}
		
		inline const SpecialValue& as_special() const {
			return as<ObjectType::Special>();
		}
		
		template<ObjectType Type>
		static PObject from(const ObjectValueType<Type>& value, uint32_t size = 0) {
			auto result = std::make_shared<Object>();
			result->set<Type>(value);
			result->array_or_map_size = size;
			return result;
		}
		
		static PObject from_bool(BoolValue value);
		
		static PObject from_int(IntValue value);
		
		static PObject from_bytes(const BytesValue& value);
		
		static PObject from_string(const std::string& value);
		
		static PObject create_array(size_t size);
		
		static PObject create_map(size_t size);
		
		static PObject from_tag(TagValue value);
		
		static PObject from_special(SpecialValue value);
		
		static PObject create_undefined();
		
		static PObject create_null();
		
		static PObject from_error(StringValue value);
		
		static PObject from_extra_int(ExtraIntValue value);
		
		static PObject from_extra_int(uint64_t value, bool sign);
		
		static PObject from_extra_tag(ExtraTagValue value);
		
		static PObject from_extra_special(ExtraSpecialValue value);
	};
}
