#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#pragma once

#include "pugixml/pugixml.hpp"
using namespace pugi;
namespace DuiLib {

	typedef pugi::xml_document CDuiXmlDoc;
	typedef pugi::xml_node CDuiXmlNode;
	typedef pugi::xml_attribute CDuiXmlAttr;
enum
{
    XMLFILE_ENCODING_UTF8 = 0,
    XMLFILE_ENCODING_UNICODE = 1,
    XMLFILE_ENCODING_ASNI = 2,
};


class UILIB_API CMarkup
{
    friend class CMarkupNode;
public:
    CMarkup(LPCTSTR pstrXML = NULL);
    ~CMarkup();

    bool Load(LPCTSTR pstrXML);
    bool LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
    bool LoadFromFile(LPCTSTR pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
    void Release();
    bool IsValid() const;

	LPCTSTR GetLastError() const;

    CDuiXmlNode GetRoot() const;

private:
	bool _Parse();
	bool _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation);
	LPTSTR m_pstrXML;
	CDuiXmlDoc m_parser;
	CDuiString m_error;
};

} // namespace DuiLib

#endif // __UIMARKUP_H__
