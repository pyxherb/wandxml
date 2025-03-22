#ifndef _WANDXML_EXCEPT_H_
#define _WANDXML_EXCEPT_H_

#include "except_base.h"
#include <new>

namespace wandxml {
	/// @brief The out of memory error, indicates that a memory allocation has failed.
	class OutOfMemoryError : public InternalException {
	public:
		WANDXML_API OutOfMemoryError();
		WANDXML_API virtual ~OutOfMemoryError();

		WANDXML_API virtual const char *what() const override;

		WANDXML_API virtual void dealloc() override;

		WANDXML_API static OutOfMemoryError *alloc();
	};

	extern OutOfMemoryError g_outOfMemoryError;

	class SyntaxError : public InternalException {
	public:
		const char *message;
		size_t off;

		WANDXML_API SyntaxError(peff::Alloc *allocator, size_t off, const char *message);
		WANDXML_API virtual ~SyntaxError();

		WANDXML_API virtual const char *what() const override;

		WANDXML_API virtual void dealloc() override;

		WANDXML_API static SyntaxError *alloc(peff::Alloc *allocator, size_t off, const char *message) noexcept;
	};

	WANDXML_FORCEINLINE InternalExceptionPointer withOutOfMemoryErrorIfAllocFailed(InternalException *exceptionPtr) noexcept {
		if (!exceptionPtr) {
			return OutOfMemoryError::alloc();
		}
		return exceptionPtr;
	}
}

#endif
