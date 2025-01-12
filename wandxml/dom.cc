#include "dom.h"

using namespace wandxml;

WANDXML_API XMLNode::XMLNode(XMLNodeType nodeType, peff::Alloc *allocator) : nodeType(nodeType), allocator(allocator) {
}

WANDXML_API XMLRegularNode::XMLRegularNode(peff::Alloc *allocator) : XMLNode(XMLNodeType::Regular, allocator), name(allocator), attributes(allocator), children(allocator) {
}

WANDXML_API XMLRegularNode::~XMLRegularNode() {
}

WANDXML_API XMLRegularNode *XMLRegularNode::alloc(peff::Alloc *allocator) noexcept {
	void *buf = allocator->alloc(sizeof(XMLRegularNode));

	if (!buf)
		return nullptr;

	new (buf) XMLRegularNode(allocator);

	return (XMLRegularNode *)buf;
}

WANDXML_API void XMLRegularNode::dealloc() {
	peff::RcObjectPtr<peff::Alloc> allocator = this->allocator;

	this->~XMLRegularNode();

	allocator->release(this);
}

WANDXML_API XMLDeclarationNode::XMLDeclarationNode(peff::Alloc *allocator) : XMLNode(XMLNodeType::Declaration, allocator), name(allocator), attributes(allocator) {
}

WANDXML_API XMLDeclarationNode::~XMLDeclarationNode() {
}

WANDXML_API XMLDeclarationNode *XMLDeclarationNode::alloc(peff::Alloc *allocator) noexcept {
	void *buf = allocator->alloc(sizeof(XMLDeclarationNode));

	if (!buf)
		return nullptr;

	new (buf) XMLDeclarationNode(allocator);

	return (XMLDeclarationNode *)buf;
}

WANDXML_API void XMLDeclarationNode::dealloc() {
	peff::RcObjectPtr<peff::Alloc> allocator = this->allocator;

	this->~XMLDeclarationNode();

	allocator->release(this);
}

WANDXML_API XMLDocumentNode::XMLDocumentNode(peff::Alloc *allocator) : XMLNode(XMLNodeType::Document, allocator), children(allocator) {
}

WANDXML_API XMLDocumentNode::~XMLDocumentNode() {
}

WANDXML_API XMLDocumentNode *XMLDocumentNode::alloc(peff::Alloc *allocator) noexcept {
	void *buf = allocator->alloc(sizeof(XMLDocumentNode));

	if (!buf)
		return nullptr;

	new (buf) XMLDocumentNode(allocator);

	return (XMLDocumentNode *)buf;
}

WANDXML_API void XMLDocumentNode::dealloc() {
	peff::RcObjectPtr<peff::Alloc> allocator = this->allocator;

	this->~XMLDocumentNode();

	allocator->release(this);
}

WANDXML_API XMLTextNode::XMLTextNode(peff::Alloc *allocator) : XMLNode(XMLNodeType::Text, allocator), value(allocator) {
}

WANDXML_API XMLTextNode::~XMLTextNode() {
}

WANDXML_API void XMLTextNode::dealloc() {
	peff::RcObjectPtr<peff::Alloc> allocator = this->allocator;

	this->~XMLTextNode();

	allocator->release(this);
}

WANDXML_API XMLTextNode* XMLTextNode::alloc(peff::Alloc* allocator) noexcept {
	void *buf = allocator->alloc(sizeof(XMLTextNode));

	if (!buf)
		return nullptr;

	new (buf) XMLTextNode(allocator);

	return (XMLTextNode *)buf;
}
