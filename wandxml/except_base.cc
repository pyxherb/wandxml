#include "except_base.h"

using namespace wandxml;

WANDXML_API InternalException::InternalException(peff::Alloc* allocator, ErrorKind kind) : allocator(allocator), kind(kind) {
}

WANDXML_API InternalException::~InternalException() {
}
