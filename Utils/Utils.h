#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include "OAIdl.h"
#include <vector>
namespace DuiLib
{

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


	template<class T>
	class UILIB_API CDuiArray
	{
	public:
		CDuiArray(){}
		~CDuiArray(){}
		void Resize(int iSize)
		{
			m_vec.resize(iSize);
		}
		bool IsEmpty() const
		{
			return m_vec.empty();
		}
		void Add(T data)
		{
			m_vec.push_back(data);
		}
		void SetAt(int iIndex, T data)
		{
			m_vec[iIndex] = data;
		}
		void InsertAt(int iIndex, T data)
		{
			m_vec.insert(m_vec.begin() + iIndex, 1, data);
		}
		void Remove(int iIndex)
		{
			m_vec.erase(m_vec.begin() + iIndex);
		}
		int GetSize() const
		{
			return m_vec.size();
		}
		LPVOID GetData()
		{
			return m_vec.data();
		}

		T GetAt(int iIndex) const
		{
			return m_vec[iIndex];
		}
		T& operator[] (int iIndex)
		{
			return m_vec[iIndex];			
		}
		void RemoveAll()
		{
			m_vec.clear();
		}
		void Append(const CDuiArray<T>& src)
		{
			m_vec.insert(m_vec.end(), src.m_vec.begin(), src.m_vec.end());
		}
	protected:
		std::vector<T> m_vec;
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;
		CStdPtrArray& operator= (const CStdPtrArray& src);

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


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