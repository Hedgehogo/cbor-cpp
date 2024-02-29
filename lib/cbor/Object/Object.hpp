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
		auto set(ObjectValueType<Type> new_value) -> void;
		
		template<ObjectType Type>
		auto as() -> ObjectValueType<Type>&;
		
		template<ObjectType Type>
		auto as() const -> ObjectValueType<Type> const&;
		
		template<ObjectType Type>
		inline auto is() const -> bool {
			return Type == static_cast<ObjectType>(value.index());
		}
		
		inline auto is_null() const -> bool {
			return is<ObjectType::Null>();
		}
		
		inline auto is_undefined() const -> bool {
			return is<ObjectType::Undefined>();
		}
		
		inline auto is_int() const -> bool {
			return is<ObjectType::Int>();
		}
		
		inline auto is_string() const -> bool {
			return is<ObjectType::String>();
		}
		
		inline auto is_bytes() const -> bool {
			return is<ObjectType::Bytes>();
		}
		
		inline auto is_bool() const -> bool {
			return is<ObjectType::Bool>();
		}
		
		inline auto is_array() const -> bool {
			return is<ObjectType::Array>();
		}
		
		inline auto is_map() const -> bool {
			return is<ObjectType::Map>();
		}
		
		inline auto is_tag() const -> bool {
			return is<ObjectType::Tag>();
		}
		
		inline auto is_special() const -> bool {
			return is<ObjectType::Special>();
		}
		
		inline auto object_type() const -> ObjectType {
			return static_cast<ObjectType>(value.index());
		}
		
		inline auto as_bool() const -> BoolValue const& {
			return as<ObjectType::Bool>();
		}
		
		inline auto as_int() const -> IntValue const& {
			return as<ObjectType::Int>();
		}
		
		inline auto as_bytes() const -> BytesValue const& {
			return as<ObjectType::Bytes>();
		}
		
		inline auto as_string() const -> StringValue const& {
			return as<ObjectType::String>();
		}
		
		inline auto as_array() const -> ArrayValue const& {
			return as<ObjectType::Array>();
		}
		
		inline auto as_map() const -> MapValue const& {
			return as<ObjectType::Map>();
		}
		
		inline auto as_tag() const -> TagValue const& {
			return as<ObjectType::Tag>();
		}
		
		inline auto as_special() const -> SpecialValue const& {
			return as<ObjectType::Special>();
		}
		
		template<ObjectType Type>
		static auto from(ObjectValueType<Type> value, uint32_t size = 0) -> PObject {
			auto result = std::make_shared<Object>();
			result->set<Type>(std::move(value));
			result->array_or_map_size = size;
			return result;
		}
		
		static auto from_bool(BoolValue value) -> PObject;
		
		static auto from_int(IntValue value) -> PObject;
		
		static auto from_bytes(BytesValue value) -> PObject;
		
		static auto from_string(std::string value) -> PObject;
		
		static auto create_array(size_t size) -> PObject;
		
		static auto create_map(size_t size) -> PObject;
		
		static auto from_tag(TagValue value) -> PObject;
		
		static auto from_special(SpecialValue value) -> PObject;
		
		static auto create_undefined() -> PObject;
		
		static auto create_null() -> PObject;
		
		static auto from_error(StringValue value) -> PObject;
		
		static auto from_extra_int(ExtraIntValue value) -> PObject;
		
		static auto from_extra_int(uint64_t value, bool sign) -> PObject;
		
		static auto from_extra_tag(ExtraTagValue value) -> PObject;
		
		static auto from_extra_special(ExtraSpecialValue value) -> PObject;
	};
}

#include "Object.inl"
