#include "parser.h"

using namespace wandxml;
using namespace wandxml::parser;

WANDXML_API void wandxml::parser::eatPrecedingWhitespaces(XMLNodeParseState *parseState) {
	while (parseState->cur < parseState->length) {
		char c = parseState->src[parseState->cur];

		switch (c) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				++parseState->cur;
				continue;
		}

		break;
	}
}

enum class StringParseState {
	Initial = 0,
	Escape
};

struct StringParseEscapeStateData {
	size_t offBeginning;
};

WANDXML_API InternalExceptionPointer parser::parseXMLString(XMLNodeParseState *parseState, peff::String &stringOut) {
	peff::ScopeGuard clearStringGuard([&stringOut]() {
		stringOut.clear();
	});
	stringOut = {};

	if (parseState->cur >= parseState->length)
		return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));

	if (parseState->src[parseState->cur] != '\"') {
		return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Expecting a string"));
	}

	++parseState->cur;

	StringParseState stringParseState = StringParseState::Initial;
	size_t idxLastEscape = parseState->cur;

	union {
		StringParseEscapeStateData asEscape;
	} stateData;

	while (parseState->cur < parseState->length) {
		char c = parseState->src[parseState->cur];

		switch (stringParseState) {
			case StringParseState::Initial:
				switch (c) {
					case '&': {
						size_t lengthToCopy = parseState->cur - idxLastEscape;
						stringParseState = StringParseState::Escape;
						stateData.asEscape.offBeginning = parseState->cur;
						if (!stringOut.resize(stringOut.size() + lengthToCopy))
							return OutOfMemoryError::alloc();
						memcpy(stringOut.data() + stringOut.size() - lengthToCopy, parseState->src + idxLastEscape, lengthToCopy);
						++parseState->cur;
						break;
					}
					case '"': {
						size_t lengthToCopy = parseState->cur - idxLastEscape;
						stringParseState = StringParseState::Escape;
						stateData.asEscape.offBeginning = parseState->cur;
						if (!stringOut.resize(stringOut.size() + lengthToCopy))
							return OutOfMemoryError::alloc();
						memcpy(stringOut.data() + stringOut.size() - lengthToCopy, parseState->src + idxLastEscape, lengthToCopy);

						clearStringGuard.release();

						++parseState->cur;

						return {};
					}
					default:
						break;
				}
				break;
			case StringParseState::Escape:
				switch (c) {
					case ';': {
						char escapedChar;

						std::string_view name;

						name = std::string_view(
							parseState->src + stateData.asEscape.offBeginning,
							parseState->cur - stateData.asEscape.offBeginning);

						if (name == "lt") {
							escapedChar = '<';
						} else if (name == "gt") {
							escapedChar = '>';
						} else if (name == "amp") {
							escapedChar = '&';
						} else if (name == "apos") {
							escapedChar = '\'';
						} else if (name == "quot") {
							escapedChar = '"';
						} else {
							return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), stateData.asEscape.offBeginning, "Invalid escape sequence"));
						}

						if (!stringOut.pushBack(std::move(escapedChar))) {
							return OutOfMemoryError::alloc();
						}

						stringParseState = StringParseState::Initial;

						idxLastEscape = parseState->cur + 1;

						break;
					}
					case '"':
						return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Prematured end of string"));
					case '\n':
						return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of line"));
					default:
						break;
				}
		}

		++parseState->cur;
	}

	return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Prematured end of file"));
}

enum class DeclarationExtractState : uint8_t {
	Initial = 0,
	EndSequenceDetect
};
WANDXML_API InternalExceptionPointer parser::extractXMLDeclaration(XMLNodeParseState *parseState, std::string_view &stringViewOut) {
	DeclarationExtractState extractState = DeclarationExtractState::Initial;
	size_t beginIndex = parseState->cur;

	for (;;) {
		if (parseState->cur >= parseState->length)
			return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));
		char c = parseState->src[parseState->cur++];

		switch (extractState) {
			case DeclarationExtractState::Initial:
				switch (c) {
					case '?':
						extractState = DeclarationExtractState::EndSequenceDetect;
						break;
					default:
						break;
				}
				break;
			case DeclarationExtractState::EndSequenceDetect:
				switch (c) {
					case '>':
						goto end;
						break;
					default:
						extractState = DeclarationExtractState::Initial;
						break;
				}
				break;
		}
	}

end:
	stringViewOut = std::string_view(parseState->src + beginIndex, parseState->cur - 2 - beginIndex);
	return {};
}

WANDXML_API InternalExceptionPointer parser::parseXMLAttribute(XMLNodeParseState *parseState, peff::String &nameOut, std::optional<peff::String> &valueOut) {
	eatPrecedingWhitespaces(parseState);

	char c;
	{
		size_t idxBeginning = parseState->cur;
		for (;;) {
			if (parseState->cur >= parseState->length)
				return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));

			c = parseState->src[parseState->cur];

			switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case '>':
				case '?':
				case '=':
					goto nameParseEnd;
			}

			++parseState->cur;
		}

	nameParseEnd:;
		if (!nameOut.resize(parseState->cur - idxBeginning))
			return OutOfMemoryError::alloc();

		memcpy(nameOut.data(), parseState->src + idxBeginning, nameOut.size());
	}

	eatPrecedingWhitespaces(parseState);

	if (parseState->cur < parseState->length) {
		switch (c) {
			case '=': {
				++parseState->cur;
				eatPrecedingWhitespaces(parseState);

				valueOut = peff::String{};

				WANDXML_RETURN_IF_EXCEPT(parseXMLString(parseState, *valueOut));

				break;
			}
			default:
				break;
		}
	}

end:
	return {};
}

WANDXML_API InternalExceptionPointer parser::parseXMLDeclaration(XMLNodeParseState *parseState, XMLDeclarationNode &declarationOut) {
	eatPrecedingWhitespaces(parseState);

	char c;
	{
		size_t idxBeginning = parseState->cur;
		for (;;) {
			if (parseState->cur >= parseState->length) {
				return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));
			}
			c = parseState->src[parseState->cur];

			switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n':
					goto nameParseEnd;
			}

			++parseState->cur;
		}

	nameParseEnd:;
		if (!declarationOut.name.resize(parseState->cur - idxBeginning))
			return OutOfMemoryError::alloc();

		memcpy(declarationOut.name.data(), parseState->src + idxBeginning, declarationOut.name.size());
	}

	eatPrecedingWhitespaces(parseState);

	while (parseState->cur < parseState->length) {
		c = parseState->src[parseState->cur];

		switch (c) {
			case '?':
				++parseState->cur;
				goto end;
			default: {
				peff::String name;
				std::optional<peff::String> value;

				WANDXML_RETURN_IF_EXCEPT(parseXMLAttribute(parseState, name, value));

				declarationOut.attributes.insert(std::move(name), std::move(value));

				eatPrecedingWhitespaces(parseState);
			}
		}
	}

end:
	if (parseState->cur >= parseState->length) {
		return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));
	}

	switch (c = parseState->src[parseState->cur]) {
		case '>':
			++parseState->cur;
			break;
		default:
			return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Expecting >"));
	}

	return {};
}

enum class RegularElementParseState {
	Contents = 0,
	ClosingTagDetect
};

WANDXML_API InternalExceptionPointer parser::parseXMLRegularElement(XMLNodeParseState *parseState, XMLRegularNode &nodeOut, bool isTopLevel) {
	char c;
	{
		size_t idxBeginning = parseState->cur;
		while (parseState->cur < parseState->length) {
			c = parseState->src[parseState->cur];

			switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case '/':
				case '>':
					goto nameParseEnd;
			}

			++parseState->cur;
		}

	nameParseEnd:;
		if (!nodeOut.name.resize(parseState->cur - idxBeginning))
			return OutOfMemoryError::alloc();

		memcpy(nodeOut.name.data(), parseState->src + idxBeginning, nodeOut.name.size());
	}

	eatPrecedingWhitespaces(parseState);

	while ((c = parseState->src[parseState->cur] != '/') && (c = parseState->src[parseState->cur] != '>')) {
		peff::String attributeName;
		std::optional<peff::String> attributeValue;
		WANDXML_RETURN_IF_EXCEPT(parseXMLAttribute(parseState, attributeName, attributeValue));

		nodeOut.attributes.insert(std::move(attributeName), std::move(attributeValue));

		eatPrecedingWhitespaces(parseState);
	}

	switch (c) {
		case '>':
			break;
		case '/': {
			if (parseState->cur >= parseState->length)
				return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));

			if (parseState->src[++parseState->cur] != '>')
				return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Expecting '>'"));

			goto end;
		}
	}

	{
		size_t idxLastValidChar = ++parseState->cur;

		{
			RegularElementParseState elementParseState = RegularElementParseState::Contents;
			for (;;) {
				if (parseState->cur >= parseState->length) {
					if (!isTopLevel) {
						return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));
					}
					goto end;
				}

				c = parseState->src[parseState->cur];

				switch (elementParseState) {
					case RegularElementParseState::Contents:
						switch (c) {
							case '<':
								elementParseState = RegularElementParseState::ClosingTagDetect;
								break;
							default:;
						}
						break;
					case RegularElementParseState::ClosingTagDetect: {
						switch (c) {
							case '/': {
								{
									size_t textLength = parseState->cur - 1 - idxLastValidChar;
									if (textLength) {
										peff::String text;
										text.resize(textLength);
										memcpy(text.data(), parseState->src + idxLastValidChar, textLength);

										// TODO: Push the text element.
										std::unique_ptr<XMLTextNode, XMLNodeDeleter> textNode(XMLTextNode::alloc(parseState->allocator.get()));
										if (!textNode)
											return OutOfMemoryError::alloc();
										textNode->value = std::move(text);
										if (!nodeOut.children.pushBack(std::unique_ptr<XMLNode, XMLNodeDeleter>(textNode.release())))
											return OutOfMemoryError::alloc();
									}
								}
								++parseState->cur;
								goto parseClosingTag;
							}
							default: {
								{
									size_t textLength = parseState->cur - 1 - idxLastValidChar;
									if (textLength) {
										peff::String text;
										text.resize(textLength);
										memcpy(text.data(), parseState->src + idxLastValidChar, textLength);

										// TODO: Push the text element.
										std::unique_ptr<XMLTextNode, XMLNodeDeleter> textNode(XMLTextNode::alloc(parseState->allocator.get()));
										if (!textNode)
											return OutOfMemoryError::alloc();
										textNode->value = std::move(text);
										if (!nodeOut.children.pushBack(std::unique_ptr<XMLNode, XMLNodeDeleter>(textNode.release())))
											return OutOfMemoryError::alloc();
									}
								}
								std::unique_ptr<XMLRegularNode, XMLNodeDeleter> childNodePtr(XMLRegularNode::alloc(parseState->allocator.get()));

								WANDXML_RETURN_IF_EXCEPT(parseXMLRegularElement(parseState, *childNodePtr.get(), false));

								if (!nodeOut.children.pushBack(std::unique_ptr<XMLNode, XMLNodeDeleter>(childNodePtr.release())))
									return OutOfMemoryError::alloc();

								elementParseState = RegularElementParseState::Contents;

								idxLastValidChar = parseState->cur;
								break;
							}
						}
						break;
					}
				}

				++parseState->cur;
			}
		}
	}

parseClosingTag:;
	{
		size_t idxBeginning = parseState->cur;
		while (parseState->cur < parseState->length) {
			c = parseState->src[parseState->cur];

			switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case '/':
				case '>':
					goto closingTagNameParseEnd;
			}

			++parseState->cur;
		}

		return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));

	closingTagNameParseEnd:;
		if (!nodeOut.name.resize(parseState->cur - idxBeginning))
			return OutOfMemoryError::alloc();

		memcpy(nodeOut.name.data(), parseState->src + idxBeginning, nodeOut.name.size());
	}

	eatPrecedingWhitespaces(parseState);

	if (parseState->cur >= parseState->length)
		return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Unexpected end of file"));

	if (parseState->src[parseState->cur] != '>')
		return withOutOfMemoryErrorIfAllocFailed(SyntaxError::alloc(parseState->allocator.get(), parseState->cur, "Expecting '>'"));
	++parseState->cur;

end:
	return {};
}

WANDXML_API InternalExceptionPointer parser::parseXMLElement(XMLNodeParseState *parseState, std::unique_ptr<XMLNode, XMLNodeDeleter> &nodeOut) {
	eatPrecedingWhitespaces(parseState);

	char c;
	size_t idxBeginning = parseState->cur;
	if (parseState->cur >= parseState->length) {
		nodeOut = nullptr;
	}

	switch ((c = parseState->src[parseState->cur])) {
		case '<': {
			++parseState->cur;

			eatPrecedingWhitespaces(parseState);

			if (parseState->cur < parseState->length) {
				switch (parseState->src[parseState->cur]) {
					case '?': {
						++parseState->cur;
						std::unique_ptr<XMLDeclarationNode, XMLNodeDeleter> declNode(XMLDeclarationNode::alloc(parseState->allocator.get()));
						WANDXML_RETURN_IF_EXCEPT(parseXMLDeclaration(parseState, *declNode.get()));
						nodeOut = std::unique_ptr<XMLNode, XMLNodeDeleter>(declNode.release());
						break;
					}
					default: {
						std::unique_ptr<XMLRegularNode, XMLNodeDeleter> newNode(XMLRegularNode::alloc(parseState->allocator.get()));
						WANDXML_RETURN_IF_EXCEPT(parseXMLRegularElement(parseState, *newNode.get()));
						nodeOut = std::unique_ptr<XMLNode, XMLNodeDeleter>(newNode.release());
						break;
					}
				}
			}

			break;
		}
		default: {
			for (;;) {
				if (parseState->cur >= parseState->length)
					break;

				switch (c = parseState->src[parseState->cur]) {
					case '<':
						goto textEnd;
					default:
						++parseState->cur;
				}
			}

		textEnd: {
			size_t textLength = parseState->cur - 1 - idxBeginning;
			if (textLength) {
				peff::String text;
				text.resize(textLength);
				memcpy(text.data(), parseState->src + idxBeginning, textLength);

				// TODO: Push the text element.
				std::unique_ptr<XMLTextNode, XMLNodeDeleter> textNode(XMLTextNode::alloc(parseState->allocator.get()));
				textNode->value = std::move(text);
				nodeOut = std::unique_ptr<XMLNode, XMLNodeDeleter>(textNode.release());
			}
		}
		}
	}

end:
	return {};
}

WANDXML_API InternalExceptionPointer parser::parseXMLDocument(XMLNodeParseState *parseState) {
	char c;
	eatPrecedingWhitespaces(parseState);

	while (parseState->cur < parseState->length) {
		std::unique_ptr<XMLNode, XMLNodeDeleter> node;
		WANDXML_RETURN_IF_EXCEPT(parseXMLElement(parseState, node));

		if (!parseState->xmlNodeOut->children.pushBack(std::move(node)))
			return OutOfMemoryError::alloc();

		eatPrecedingWhitespaces(parseState);
	}

	return {};
}

InternalExceptionPointer wandxml::parseXMLNode(peff::Alloc *allocator, const char *src, size_t length, std::unique_ptr<XMLNode, XMLNodeDeleter> &xmlNodeOut) {
	parser::XMLNodeParseState parseState;
	char c;

	parseState.src = src;
	parseState.cur = 0;
	parseState.length = length;
	parseState.allocator = allocator;

	if (!(parseState.xmlNodeOut = std::unique_ptr<XMLDocumentNode, XMLNodeDeleter>(XMLDocumentNode::alloc(allocator))))
		return OutOfMemoryError::alloc();

	WANDXML_RETURN_IF_EXCEPT(parseXMLDocument(&parseState));

	xmlNodeOut = std::move(parseState.xmlNodeOut);

	return {};
}
