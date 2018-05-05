#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include "OAIdl.h"
namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(int x, int y);
		CDuiPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	//只支持基本类型，不支持类，因为里面不调用构造函数
	template<class T>
	class UILIB_API CDuiArray
	{
	public:
		CDuiArray(int iPreallocSize = 0) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
		{
//			ASSERT(iPreallocSize >= 0);
			if (iPreallocSize > 0)
			{
				Resize(iPreallocSize);
			}
		}
		CDuiArray(const CDuiArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
		{
			for (int i = 0; i < src.GetSize(); i++)
				Add(src.GetAt(i));
		}
		~CDuiArray()
		{
			if (m_ppVoid != NULL) free(m_ppVoid);
		}

		void Empty()
		{
			if (m_ppVoid != NULL) free(m_ppVoid);
			m_ppVoid = NULL;
			m_nCount = m_nAllocated = 0;
		}
		void Resize(int iSize)
		{
			Empty();
			m_ppVoid = static_cast<T*>(malloc(iSize * sizeof(T)));
			::ZeroMemory(m_ppVoid, iSize * sizeof(T));
			m_nAllocated = iSize;
			m_nCount = iSize;
		}
		bool IsEmpty() const
		{
			return m_nCount == 0;
		}

		int Find(T tData) const
		{
			for (int i = 0; i < m_nCount; i++) if (m_ppVoid[i] == tData) return i;
			return -1;
		}
		bool Add(T tData)
		{
			if (++m_nCount >= m_nAllocated) {
				int nAllocated = m_nAllocated * 2;
				if (nAllocated == 0) nAllocated = 11;
				T* ppVoid = static_cast<T*>(realloc(m_ppVoid, nAllocated * sizeof(T)));
				if (ppVoid != NULL) {
					m_nAllocated = nAllocated;
					m_ppVoid = ppVoid;
				}
				else {
					--m_nCount;
					return false;
				}
			}
			m_ppVoid[m_nCount - 1] = tData;
			return true;
		}
		bool SetAt(int iIndex, T tData)
		{
			if (iIndex < 0 || iIndex >= m_nCount) return false;
			m_ppVoid[iIndex] = tData;
			return true;
		}
		bool InsertAt(int iIndex, T tData)
		{
			if (iIndex == m_nCount) return Add(tData);
			if (iIndex < 0 || iIndex > m_nCount) return false;
			if (++m_nCount >= m_nAllocated) {
				int nAllocated = m_nAllocated * 2;
				if (nAllocated == 0) nAllocated = 11;
				LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
				if (ppVoid != NULL) {
					m_nAllocated = nAllocated;
					m_ppVoid = ppVoid;
				}
				else {
					--m_nCount;
					return false;
				}
			}
			memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
			m_ppVoid[iIndex] = tData;
			return true;
		}
		bool Remove(int iIndex)
		{
			if (iIndex < 0 || iIndex >= m_nCount) return false;
			if (iIndex < --m_nCount) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
			return true;
		}
		int GetSize() const
		{
			return m_nCount;
		}

		T* GetData()
		{
			return m_ppVoid;
		}

		T GetAt(int iIndex) const
		{
			if (iIndex < 0 || iIndex >= m_nCount) return NULL;
			return m_ppVoid[iIndex];
		}
		T operator[] (int iIndex) const
		{
			//ASSERT(iIndex >= 0 && iIndex < m_nCount);
			return m_ppVoid[iIndex];
		}

	protected:
		T * m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};

	typedef CDuiArray<LPVOID> CDuiPtrArray;


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		CDuiString();
		CDuiString(const TCHAR ch);
		CDuiString(const CDuiString& src);
		CDuiString(LPCTSTR lpsz, int nLen = -1);
		~CDuiString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

		TCHAR operator[] (int nIndex) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const CDuiString& CDuiString::operator=(LPCSTR lpStr);
		const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
		const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
		const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
		CDuiString operator+(const CDuiString& src) const;
		CDuiString operator+(LPCTSTR pstr) const;
		const CDuiString& operator+=(const CDuiString& src);
		const CDuiString& operator+=(LPCTSTR pstr);
		const CDuiString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CDuiString Left(int nLength) const;
		CDuiString Mid(int iPos, int nLength = -1) const;
		CDuiString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		int __cdecl Format(LPCTSTR pstrFormat, ...);
        int __cdecl Format(LPCTSTR pstrFormat, va_list Args);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);
		int Delete(int iPos, int iCount = 1);
	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiWaitCursor
	{
	public:
		CDuiWaitCursor();
		~CDuiWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};

	class CCriticalSection
	{
	public:
		CCriticalSection()
		{
			InitializeCriticalSection(&m_CriticalSection);
		}
		~CCriticalSection()
		{
			DeleteCriticalSection(&m_CriticalSection);
		}
		void Enter()
		{
			EnterCriticalSection(&m_CriticalSection);
		}
		void Leave()
		{
			LeaveCriticalSection(&m_CriticalSection);
		}
		CRITICAL_SECTION m_CriticalSection;

	};

	class CAutoCriticalSection
	{
	public:
		CAutoCriticalSection(CCriticalSection& sec) : m_sec(sec)
		{
			m_sec.Enter();
		}
		~CAutoCriticalSection()
		{
			m_sec.Leave();
		}
		CCriticalSection& m_sec;
	};
}// namespace DuiLib

#endif // __UTILS_H__