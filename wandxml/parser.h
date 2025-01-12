#ifndef _WANDXML_PARSER_H_
#define _WANDXML_PARSER_H_

#include "dom.h"
#include <optional>

namespace wandxml {
	namespace parser {
		struct XMLNodeParseState {
			peff::RcObjectPtr<peff::Alloc> allocator;
			std::unique_ptr<XMLDocumentNode, XMLNodeDeleter> xmlNodeOut;
			const char *src;
			size_t baseOff = 0;
			size_t cur;
			size_t length;
		};

		WANDXML_API void eatPrecedingWhitespaces(XMLNodeParseState *parseState);

		WANDXML_API InternalExceptionPointer parseXMLString(XMLNodeParseState *parseState, peff::String &stringOut);

		/// @brief Extract an XML declaration from following string.
		/// @param parseState Parse state to be used.
		/// @param stringViewOut Where to store the extracted declaration.
		/// @return nullptr if succeeded, or an exception raised.
		WANDXML_API InternalExceptionPointer extractXMLDeclaration(XMLNodeParseState *parseState, std::string_view &stringViewOut);

		WANDXML_API InternalExceptionPointer extractXMLOpeningTag(XMLNodeParseState *parseState, std::string_view &stringViewOut);

		WANDXML_API InternalExceptionPointer parseXMLAttribute(XMLNodeParseState *parseState, peff::String &nameOut, std::optional<peff::String> &valueOut);

		WANDXML_API InternalExceptionPointer parseXMLDeclaration(XMLNodeParseState *parseState, XMLDeclarationNode &declarationOut);

		WANDXML_API InternalExceptionPointer parseXMLRegularElement(XMLNodeParseState *parseState, XMLRegularNode &nodeOut, bool isTopLevel = true);

		WANDXML_API InternalExceptionPointer parseXMLElement(XMLNodeParseState *parseState, std::unique_ptr<XMLNode, XMLNodeDeleter> &nodeOut);

		WANDXML_API InternalExceptionPointer parseXMLDocument(XMLNodeParseState *parseState);
	}

	WANDXML_API InternalExceptionPointer parseXMLNode(peff::Alloc *allocator, const char *src, size_t length, std::unique_ptr<XMLNode, XMLNodeDeleter> &xmlNodeOut);
}

#endif
