#pragma once

#include <string>
#include <exception>
#include <memory>
#include <cstdint>
#include <cstdint>

namespace cbor {
	constexpr auto default_exception_code = 1;
	
	class Exception : public std::exception {
	protected:
		int64_t code_;
		std::string name_value_;
		std::string error_msg_;
	
	public:
		inline Exception(int64_t code, std::string name_value, std::string error_msg) :
			code_(code), name_value_(std::move(name_value)), error_msg_(std::move(error_msg)) {
		}
		
		inline Exception(std::string msg) :
			code_(default_exception_code), error_msg_(std::move(msg)) {
		}
		
		inline Exception(char const* msg) :
			code_(default_exception_code), error_msg_(msg) {
		}
		
		inline Exception() : code_(0) {
		}
		
		inline Exception(Exception const& other) = default;
		
		inline auto name() const -> std::string {
			return name_value_;
		}
		
		inline virtual auto code() const -> int64_t {
			return code_;
		}
		
		inline virtual auto what() const noexcept -> char const* {
			return error_msg_.c_str();
		}
		
		inline virtual auto dynamic_copy_exception() const -> std::shared_ptr<Exception> {
			return std::make_shared<Exception>(*this);
		}
		
		inline virtual auto dynamic_rethrow_exception() const -> void {
			if(code() == 0) {
				throw *this;
			} else {
				Exception::dynamic_rethrow_exception();
			}
		}
		
		inline auto operator=(const Exception& other) -> Exception& {
			if(this != &other) {
				error_msg_ = other.error_msg_;
			}
			return *this;
		}
		
		inline virtual ~Exception() = default;
	};
	
	template<typename Helper_, typename Base_, int64_t Code_>
	class DeclareDerivedException : public Base_ {
	public:
		explicit DeclareDerivedException(int64_t code, std::string name_value, std::string what_value) :
			Base_(code, std::move(name_value), std::move(what_value)) {
		}
		
		explicit DeclareDerivedException(std::string what_value) :
			Base_(Code_, Helper_::type_str, std::move(what_value)) {
		}
		
		explicit DeclareDerivedException() :
			Base_(Code_, Helper_::type_str, Helper_::what) {
		}
		
		virtual auto dynamic_copy_exception() const -> std::shared_ptr<Exception> {
			return std::make_shared<DeclareDerivedException<Helper_, Base_, Code_> >(*this);
		}
		
		virtual auto dynamic_rethrow_exception() const -> void {
			if(this->code() == Code_) {
				throw *this;
			} else {
				cbor::Exception::dynamic_rethrow_exception();
			}
		}
	};
	
	template<typename Helper_, int64_t Code_>
	using DeclareException = DeclareDerivedException<Helper_, Exception, Code_>;
	
	struct EncodeExceptionHelper {
		constexpr static auto type_str{"EncodeException"};
		
		constexpr static auto what{"CBOR Encode exception"};
	};
	
	using EncodeException = DeclareException<EncodeExceptionHelper, 101>;
	
	struct DecodeExceptionHelper {
		constexpr static auto type_str {"DecodeException"};
		
		constexpr static auto what {"CBOR Decode exception"};
	};
	
	using DecodeException = DeclareException<EncodeExceptionHelper, 102>;
	
	struct OutputExceptionHelper {
		constexpr static auto type_str {"OutputException"};
		
		constexpr static auto what {"CBOR Output exception"};
	};
	
	using OutputException = DeclareException<OutputExceptionHelper, 103>;
}
