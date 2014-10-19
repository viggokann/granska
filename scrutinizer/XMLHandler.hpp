#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>


using namespace xercesc;
using namespace std;

class XMLHandler : public DefaultHandler {
public:
	XMLHandler();
	~XMLHandler();
    void startElement(
        const   XMLCh* const    uri,
        const   XMLCh* const    localname,
        const   XMLCh* const    qname,
        const   Attributes&     attrs
    );
    
    void endElement(
        const   XMLCh* const    uri,
        const   XMLCh* const    localname,
        const   XMLCh* const    qname
    );
    
    void characters(const   XMLCh* const		chars, const XMLSize_t length);

    void fatalError(const SAXParseException&);
    
    void endDocument();
    
private:
		bool found;
		int sar, stavning, dom, pronomen, kong, verb, 
		tempus, fragetecken, utropstecken, egennamn,
		nymening, gemen , ordbi, ordind, kompsuffix, kompperi, ljud;
};
