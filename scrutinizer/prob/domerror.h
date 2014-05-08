#ifndef DOMERROR_H
#define DOMERROR_H

//#include <xercesc/sax/SAXParseException.hpp>
//#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <iostream>

//class SAXParseException;

using namespace xercesc;

// ---------------------------------------------------------------------------
// Simple error handler deriviative to install on parser
// ---------------------------------------------------------------------------

class DOMCountErrorHandler : public HandlerBase
{
public:
    // -----------------------------------------------------------------------
    // Constructors and Destructor
    // -----------------------------------------------------------------------
    DOMCountErrorHandler();
    ~DOMCountErrorHandler();


    // -----------------------------------------------------------------------
    // Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;


    // -----------------------------------------------------------------------
    // Implementation of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& e);
    void error(const SAXParseException& e);
    void fatalError(const SAXParseException& e);
    void resetErrors();


private :
    // -----------------------------------------------------------------------
    // Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    DOMCountErrorHandler(const DOMCountErrorHandler&);
    void operator=(const DOMCountErrorHandler&);


    // -----------------------------------------------------------------------
    // Private data members
    //
    // fSawErrors
    // This is set if we get any errors, and is queryable via a getter
    // method. Its used by the main code to suppress output if there are
    // errors.
    // -----------------------------------------------------------------------
    bool fSawErrors;
};


// ---------------------------------------------------------------------------
// This is a simple class that lets us do easy (though not terribly efficient)
// trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    // Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        //delete [] fLocalForm;
    }


    // -----------------------------------------------------------------------
    // Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private :
    // -----------------------------------------------------------------------
    // Private data members
    //
    // fLocalForm
    // This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char* fLocalForm;
};

inline std::ostream& operator<<(std::ostream & target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}

inline bool DOMCountErrorHandler::getSawErrors() const
{
    return fSawErrors;
}

inline DOMCountErrorHandler::DOMCountErrorHandler() :

    fSawErrors(false)
{
}

inline DOMCountErrorHandler::~DOMCountErrorHandler()
{
}


// ---------------------------------------------------------------------------
// DOMCountHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
inline void DOMCountErrorHandler::error(const SAXParseException& e)
{
    fSawErrors = true;
    std::cerr << "\nError at file " << StrX(e.getSystemId())
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n Message: " << StrX(e.getMessage()) << std::endl;
}

inline void DOMCountErrorHandler::fatalError(const SAXParseException& e)
{
    fSawErrors = true;
    std::cerr << "\nFatal Error at file " << StrX(e.getSystemId())
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n Message: " << StrX(e.getMessage()) << std::endl;
}

inline void DOMCountErrorHandler::warning(const SAXParseException& e)
{
    std::cerr << "\nWarning at file " << StrX(e.getSystemId())
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n Message: " << StrX(e.getMessage()) << std::endl;
}

inline void DOMCountErrorHandler::resetErrors()
{
}


#endif // DOMERROR_H
