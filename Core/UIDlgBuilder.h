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
    CDialogBuilder(CPaintManagerUI* pManager);

	void SetAttrbuteCallBack(IDialogBuilderAttrbuteCallback * pAttrbuteCallback = NULL);

    CControlUI* Create(STRINGorID xml, LPCTSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
        CPaintManagerUI* pManager = NULL, CControlUI* pParent = NULL);

    CControlUI* Create(IDialogBuilderCallback* pCallback = NULL, CPaintManagerUI* pManager = NULL,
        CControlUI* pParent = NULL);

    CMarkup* GetMarkup();

    void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;
private:
    CControlUI* _Parse(CMarkupNode* parent, CControlUI* pParent = NULL, CPaintManagerUI* pManager = NULL);

    CMarkup m_xml;
    IDialogBuilderCallback* m_pCallback;
    LPCTSTR m_pstrtype;
	IDialogBuilderAttrbuteCallback * m_pAttrbuteCallback;
	CPaintManagerUI* m_pManager;
};

} // namespace DuiLib

#endif // __UIDLGBUILDER_H__
