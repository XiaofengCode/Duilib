#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#pragma once

namespace DuiLib {

class IDialogBuilderCallback
{
public:
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) = 0;
};

class IDialogBuilderAttrbuteCallback
{
public:
	virtual BOOL SetAttribute(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue) = 0;
};


class UILIB_API CDialogBuilder
{
public:
	LBIND_BASE_CLASS_DEFINE(CDialogBuilder);
    CDialogBuilder();

	void SetAttrbuteCallBack(IDialogBuilderAttrbuteCallback * pAttrbuteCallback = NULL);

    CControlUI* Create(STRINGorID xml, LPCTSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
        CPaintManagerUI* pManager = NULL, CControlUI* pParent = NULL);

    CControlUI* Create(IDialogBuilderCallback* pCallback = NULL, CPaintManagerUI* pManager = NULL,
        CControlUI* pParent = NULL);

	void _ParseEvent(CDuiXmlNode &node, CPaintManagerUI* pManager, CControlUI* pParent);

	void _ParseDefault(CDuiXmlNode &node, CPaintManagerUI* pManager);

	void _ParseFont(CPaintManagerUI* pManager, CDuiXmlNode &node);

	void _ParseImage(CDuiXmlNode &node, CPaintManagerUI* pManager);

	void _ParseWindow(CPaintManagerUI* pManager, CDuiXmlNode &root);

    CMarkup* GetMarkup();

	bool LoadFile(LPCTSTR file);
	bool LoadString(LPCWSTR str);
	bool LoadString(LPCSTR str);


//     void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
//     void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;
private:
    CControlUI* _ParseControl(CDuiXmlNode* parent, CControlUI* pParent = NULL, CPaintManagerUI* pManager = NULL);

	void _ParseInclude(CDuiXmlNode &node, CPaintManagerUI* pManager, CControlUI* pParent);

    CMarkup m_xml;
    IDialogBuilderCallback* m_pCallback;
    LPCTSTR m_pstrtype;
	IDialogBuilderAttrbuteCallback * m_pAttrbuteCallback;
};

} // namespace DuiLib

#endif // __UIDLGBUILDER_H__
