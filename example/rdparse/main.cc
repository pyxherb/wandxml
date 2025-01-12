#include <wandxml/parser.h>
#include <fstream>

void dumpXMLNode(wandxml::XMLNode *node, size_t indentLevel = 0) {
	switch (node->nodeType) {
		case wandxml::XMLNodeType::Document: {
			wandxml::XMLDocumentNode *docNode = (wandxml::XMLDocumentNode *)node;

			for (size_t i = 0; i < docNode->children.size(); ++i) {
				dumpXMLNode(docNode->children.at(i).get(), indentLevel);
			}
			break;
		}
		case wandxml::XMLNodeType::Regular: {
			wandxml::XMLRegularNode *regularNode = (wandxml::XMLRegularNode *)node;

			for (size_t i = 0; i < indentLevel; ++i)
				putchar(' ');

			printf("<%s ", regularNode->name.data());

			for (auto i = regularNode->attributes.beginConst();
				 i != regularNode->attributes.endConst();
				 ++i) {
				printf("%s", i.key().data());

				if (i.value().has_value()) {
					printf("=\"%s\"", (*i.value()).data());
				}

				putchar(' ');
			}

			puts(">");

			for (size_t i = 0; i < regularNode->children.size(); ++i) {
				dumpXMLNode(regularNode->children.at(i).get(), indentLevel + 1);
			}

			for (size_t i = 0; i < indentLevel; ++i)
				putchar(' ');

			printf("</%s>\n", regularNode->name.data());
			break;
		}
		case wandxml::XMLNodeType::Declaration: {
			wandxml::XMLDeclarationNode *declNode = (wandxml::XMLDeclarationNode *)node;

			for (size_t i = 0; i < indentLevel; ++i)
				putchar(' ');

			printf("<?%s ", declNode->name.data());

			for (auto i = declNode->attributes.beginConst();
				 i != declNode->attributes.endConst();
				 ++i) {
				printf("%s", i.key().data());

				if (i.value().has_value()) {
					printf("=\"%s\"", (*i.value()).data());
				}

				putchar(' ');
			}

			puts("?>");
			break;
		}
		case wandxml::XMLNodeType::Text: {
			wandxml::XMLTextNode *textNode = (wandxml::XMLTextNode *)node;

			for (size_t i = 0; i < indentLevel; ++i)
				putchar(' ');

			printf("\"%s\"\n", textNode->value.data());
			break;
		}
	}
}

int main() {
	std::ifstream is("test.xml");

	is.seekg(0, std::ios::end);
	size_t size = is.tellg();
	is.seekg(0, std::ios::beg);

	std::unique_ptr<char[]> testXml(std::make_unique<char[]>(size));
	is.read(testXml.get(), size);

	std::unique_ptr<wandxml::XMLNode, wandxml::XMLNodeDeleter> xmlNode;
	wandxml::parseXMLNode(peff::getDefaultAlloc(), testXml.get(), size, xmlNode);

	dumpXMLNode(xmlNode.get());

	return 0;
}
