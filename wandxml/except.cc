#include "except.h"

using namespace wandxml;

OutOfMemoryError wandxml::g_outOfMemoryError;

WANDXML_API OutOfMemoryError::OutOfMemoryError() : InternalException(nullptr, ErrorKind::OutOfMemory) {}
WANDXML_API OutOfMemoryError::~OutOfMemoryError() {}

WANDXML_API const char *OutOfMemoryError::what() const {
	return "Out of memory";
}

WANDXML_API void OutOfMemoryError::dealloc() {
}

WANDXML_API OutOfMemoryError *OutOfMemoryError::alloc() {
	return &g_outOfMemoryError;
}

WANDXML_API SyntaxError::SyntaxError(peff::Alloc *allocator, size_t off, const char *message)
	: InternalException(allocator, ErrorKind::SyntaxError),
	  off(off),
	  message(message) {}
WANDXML_API SyntaxError::~SyntaxError() {}

WANDXML_API const char *SyntaxError::what() const {
	return message;
}

WANDXML_API void SyntaxError::dealloc() {
	this->~SyntaxError();
	allocator->release((void *)this);
}

WANDXML_API SyntaxError *SyntaxError::alloc(peff::Alloc *allocator, size_t off, const char *message) noexcept {
	void *buf = allocator->alloc(sizeof(SyntaxError));

	if (!buf)
		return nullptr;

	new (buf) SyntaxError(allocator, off, message);

	return (SyntaxError *)buf;
}
