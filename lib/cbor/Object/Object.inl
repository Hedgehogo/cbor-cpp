//included into Object.hpp

namespace cbor {
	template<ObjectType Type>
	auto Object::set(ObjectValueType<Type> new_value) -> void {
		value.emplace<static_cast<size_t>(Type)>(std::move(new_value));
	}
	
	template<ObjectType Type>
	auto Object::as() -> ObjectValueType<Type>& {
		return std::get<static_cast<size_t>(Type)>(value);
	}
	
	template<ObjectType Type>
	auto Object::as() const -> ObjectValueType<Type> const& {
		return std::get<static_cast<size_t>(Type)>(value);
	}
}
