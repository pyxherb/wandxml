#ifndef _WANDXML_DOM_H_
#define _WANDXML_DOM_H_

#include "except.h"
#include <peff/containers/string.h>
#include <peff/containers/hashmap.h>
#include <peff/containers/dynarray.h>
#include <optional>

namespace wandxml {
	enum class XMLNodeType {
		Regular = 0,
		Declaration,
		Comment,
		Text,
		Document
	};

	class XMLNode {
	public:
		XMLNodeType nodeType;
		peff::RcObjectPtr<peff::Alloc> allocator;

		WANDXML_API XMLNode(XMLNodeType nodeType, peff::Alloc *allocator);

		virtual void dealloc() = 0;
	};

	struct XMLNodeDeleter {
		void operator()(XMLNode *ptr) {
			if (ptr) {
				ptr->dealloc();
			}
		}
	};

	class XMLRegularNode : public XMLNode {
	public:
		peff::String name;
		peff::HashMap<peff::String, std::optional<peff::String>> attributes;
		peff::DynArray<std::unique_ptr<XMLNode, XMLNodeDeleter>> children;

		XMLRegularNode(const XMLRegularNode &) = delete;
		XMLRegularNode &operator=(const XMLRegularNode &) = delete;

		WANDXML_API XMLRegularNode(peff::Alloc *allocator);
		WANDXML_API virtual ~XMLRegularNode();

		WANDXML_API virtual void dealloc() override;

		WANDXML_API static XMLRegularNode *alloc(peff::Alloc *allocator) noexcept;
	};

	class XMLDeclarationNode : public XMLNode {
	public:
		peff::String name;
		peff::HashMap<peff::String, std::optional<peff::String>> attributes;

		WANDXML_API XMLDeclarationNode(peff::Alloc *allocator);
		WANDXML_API virtual ~XMLDeclarationNode();

		WANDXML_API virtual void dealloc() override;

		WANDXML_API static XMLDeclarationNode *alloc(peff::Alloc *allocator) noexcept;
	};

	class XMLCommentNode : public XMLNode {
	public:
		peff::String content;

		WANDXML_API XMLCommentNode(peff::Alloc *allocator);

		WANDXML_API virtual void dealloc() override;
	};

	class XMLTextNode : public XMLNode {
	public:
		peff::String value;

		WANDXML_API XMLTextNode(peff::Alloc *allocator);
		WANDXML_API virtual ~XMLTextNode();

		WANDXML_API virtual void dealloc() override;

		WANDXML_API static XMLTextNode *alloc(peff::Alloc *allocator) noexcept;
	};

	class XMLDocumentNode : public XMLNode {
	public:
		peff::DynArray<std::unique_ptr<XMLNode, XMLNodeDeleter>> children;

		WANDXML_API XMLDocumentNode(peff::Alloc *allocator);
		WANDXML_API virtual ~XMLDocumentNode();

		WANDXML_API virtual void dealloc() override;

		WANDXML_API static XMLDocumentNode *alloc(peff::Alloc *allocator) noexcept;
	};
}

#endif
