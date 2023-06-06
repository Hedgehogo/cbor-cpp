#include "Object.hpp"

namespace cbor {
	PObject Object::from_bool(BoolValue value) {
		return from<ObjectType::Bool>(value);
	}
	
	PObject Object::from_int(IntValue value) {
		return from<ObjectType::Int>(value);
	}
	
	PObject Object::from_bytes(const BytesValue& value) {
		return from<ObjectType::Bytes>(value);
	}
	
	PObject Object::from_string(const std::string& value) {
		return from<ObjectType::String>(value);
	}
	
	PObject Object::create_array(size_t size) {
		return from<ObjectType::Array>(ArrayValue{}, size);
	}
	
	PObject Object::create_map(size_t size) {
		return from<ObjectType::Map>(MapValue{}, size);
	}
	
	PObject Object::from_tag(TagValue value) {
		return from<ObjectType::Tag>(value);
	}
	
	PObject Object::create_undefined() {
		return from<ObjectType::Undefined>(std::monostate{});
	}
	
	PObject Object::create_null() {
		return from<ObjectType::Null>(std::monostate{});
	}
	
	PObject Object::from_special(SpecialValue value) {
		return from<ObjectType::Special>(value);
	}
	
	PObject Object::from_error(StringValue value) {
		return from<ObjectType::Error>(value);
	}
	
	PObject Object::from_extra_int(uint64_t value, bool sign) {
		return from<ObjectType::ExtraInt>({sign, value});
	}
	
	PObject Object::from_extra_tag(ExtraTagValue value) {
		return from<ObjectType::ExtraTag>(value);
	}
	
	PObject Object::from_extra_special(ExtraSpecialValue value) {
		return from<ObjectType::ExtraSpecial>(value);
	}
}