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
		int64_t _code;
		std::string _name_value;
		std::string _error_msg;
	
	public:
		inline Exception() : _code(0) {
		}
		
		inline Exception(int64_t code, const std::string& name_value, const std::string& error_msg)
			: _code(code), _name_value(name_value), _error_msg(error_msg) {
		}
		
		inline Exception(const Exception& other) {
			_code = other._code;
			_name_value = other._name_value;
			_error_msg = other._error_msg;
		}
		
		inline Exception(const char* msg) {
			_code = default_exception_code;
			_error_msg = msg;
		}
		
		inline Exception(const std::string& msg) {
			_code = default_exception_code;
			_error_msg = msg;
		}
		
		inline Exception& operator=(const Exception& other) {
			if(this != &other) {
				_error_msg = other._error_msg;
			}
			return *this;
		}
		
		inline virtual ~Exception() {
		}
		
		inline virtual const char* what() const noexcept {
			return _error_msg.c_str();
		}
		
		inline virtual int64_t code() const {
			return _code;
		}
		
		inline std::string name() const {
			return _name_value;
		}
		
		inline virtual std::shared_ptr<Exception> dynamic_copy_exception() const {
			return std::make_shared<Exception>(*this);
		}
		
		inline virtual void dynamic_rethrow_exception() const {
			if(code() == 0)
				throw *this;
			else
				Exception::dynamic_rethrow_exception();
		}
	};
	
	template<typename Helper_, typename Base_, int64_t Code_>
	class DeclareDerivedException : public Base_ {
	public:
		explicit DeclareDerivedException(int64_t code, const std::string& name_value, const std::string& what_value) :
			Base_(code, name_value, what_value) {
		}
		
		explicit DeclareDerivedException() :
			Base_(Code_, Helper_::type_str(), Helper_::what()) {
		}
		
		virtual std::shared_ptr<Exception> dynamic_copy_exception() const {
			return std::make_shared<DeclareDerivedException<Helper_, Base_, Code_> >(*this);
		}
		
		virtual void dynamic_rethrow_exception() const {
			if(this->code() == Code_) {
				throw *this;
			} else {
				cbor::Exception::dynamic_rethrow_exception();
			}
		}
		
		explicit DeclareDerivedException(const std::string& what_value) :
			Base_(Code_, Helper_::type_str(), what_value) {
		}
	};
	
	template<typename Helper_, int64_t Code_>
	using DeclareException = DeclareDerivedException<Helper_, Exception, Code_>;
	
	struct EncodeExceptionHelper {
		static std::string type_str() {
			return "EncodeException";
		};
		
		static std::string what() {
			return "CBOR Encode exception";
		};
	};
	
	using EncodeException = DeclareException<EncodeExceptionHelper, 101>;
	
	struct DecodeExceptionHelper {
		static std::string type_str() {
			return "DecodeException";
		};
		
		static std::string what() {
			return "CBOR Decode exception";
		};
	};
	
	using DecodeException = DeclareException<EncodeExceptionHelper, 102>;
	
	struct OutputExceptionHelper {
		static std::string type_str() {
			return "OutputException";
		};
		
		static std::string what() {
			return "CBOR Output exception";
		};
	};
	
	using OutputException = DeclareException<OutputExceptionHelper, 103>;
}
