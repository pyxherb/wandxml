#ifndef _WANDXML_EXCEPT_BASE_H_
#define _WANDXML_EXCEPT_BASE_H_

#include "basedefs.h"
#include <peff/base/alloc.h>

namespace wandxml {
	enum class ErrorKind {
		OutOfMemory = 0,
		SyntaxError
	};

	class InternalException {
	public:
		mutable peff::RcObjectPtr<peff::Alloc> allocator;
		ErrorKind kind;

		WANDXML_API InternalException(peff::Alloc *allocator, ErrorKind kind);
		WANDXML_API virtual ~InternalException();

		virtual const char *what() const = 0;

		virtual void dealloc() = 0;
	};

	class InternalExceptionPointer {
	private:
		InternalException *_ptr = nullptr;

	public:
		WANDXML_FORCEINLINE InternalExceptionPointer() noexcept = default;
		WANDXML_FORCEINLINE InternalExceptionPointer(InternalException *exception) noexcept : _ptr(exception) {
		}

		WANDXML_FORCEINLINE ~InternalExceptionPointer() noexcept {
			unwrap();
			reset();
		}

		InternalExceptionPointer(const InternalExceptionPointer &) = delete;
		InternalExceptionPointer &operator=(const InternalExceptionPointer &) = delete;
		WANDXML_FORCEINLINE InternalExceptionPointer(InternalExceptionPointer &&other) noexcept {
			_ptr = other._ptr;
			other._ptr = nullptr;
		}
		WANDXML_FORCEINLINE InternalExceptionPointer &operator=(InternalExceptionPointer &&other) noexcept {
			_ptr = other._ptr;
			other._ptr = nullptr;
			return *this;
		}

		WANDXML_FORCEINLINE InternalException *get() noexcept {
			return _ptr;
		}
		WANDXML_FORCEINLINE const InternalException *get() const noexcept {
			return _ptr;
		}

		WANDXML_FORCEINLINE void reset() noexcept {
			if (_ptr) {
				_ptr->dealloc();
			}
			_ptr = nullptr;
		}

		WANDXML_FORCEINLINE void unwrap() noexcept {
			if (_ptr) {
				assert(("Unhandled WandXML internal exception: ", false));
			}
		}

		WANDXML_FORCEINLINE explicit operator bool() noexcept {
			return (bool)_ptr;
		}

		WANDXML_FORCEINLINE InternalException *operator->() noexcept {
			return _ptr;
		}

		WANDXML_FORCEINLINE const InternalException *operator->() const noexcept {
			return _ptr;
		}
	};
}

#define WANDXML_UNWRAP_EXCEPT(expr) (expr).unwrap()
#define WANDXML_RETURN_IF_EXCEPT(expr)                         \
	if (wandxml::InternalExceptionPointer e = (expr); (bool)e) \
	return e
#define WANDXML_RETURN_IF_EXCEPT_WITH_LVAR(name, expr) \
	if ((bool)(name = (expr)))                         \
		return name;

#endif
