#include "Object.hpp"

namespace cbor {
	PObject Object::from_int(IntValue value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Int;
		result->value = value;
		return result;
	}
	
	PObject Object::from_bool(BoolValue value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Bool;
		result->value = value;
		return result;
	}
	
	PObject Object::from_bytes(const BytesValue& value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Bytes;
		result->value = value;
		return result;
	}
	
	PObject Object::from_string(const std::string& value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::String;
		result->value = value;
		return result;
	}
	
	PObject Object::create_array(size_t size) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Array;
		result->value = ArrayValue();
		result->array_or_map_size = size;
		return result;
	}
	
	PObject Object::create_map(size_t size) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Map;
		result->value = MapValue();
		result->array_or_map_size = size;
		return result;
	}
	
	PObject Object::from_tag(TagValue value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Tag;
		result->value = value;
		return result;
	}
	
	PObject Object::create_undefined() {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Undefined;
		return result;
	}
	
	PObject Object::create_null() {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Null;
		return result;
	}
	
	PObject Object::from_special(uint32_t value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::Special;
		result->value = value;
		return result;
	}
	
	PObject Object::from_extra_integer(uint64_t value, bool sign) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::ExtraInt;
		result->value = value;
		result->is_positive_extra = sign;
		return result;
	}
	
	PObject Object::from_extra_tag(uint64_t value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::ExtraTag;
		result->value = value;
		return result;
	}
	
	PObject Object::from_extra_special(uint64_t value) {
		auto result = std::make_shared<Object>();
		result->type = ObjectType::ExtraSpecial;
		result->value = value;
		return result;
	}
}