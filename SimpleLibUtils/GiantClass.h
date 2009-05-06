//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Giant.h - declaration of CGiant

#ifndef __GIANT_H
#define __GIANT_H

extern "C"
{
#include "giants.h"
}

namespace Simple
{

// Utility functions
bool gparse(giant g, const wchar_t* psz, int iBase);
CUniString gformat(giant g, int iBase);

template <int iSize>
class CGiantBase
{
public:
	CGiantBase()
	{
		m_p=newgiant(iSize);
	}

	CGiantBase(int iVal)
	{
		m_p=newgiant(iSize);
		itog(iVal, m_p);
	}
	CGiantBase(const wchar_t* psz, int iBase)
	{
		m_p=newgiant(iSize);
		gparse(m_p, psz, iBase);
	}
	CGiantBase(const CGiantBase<iSize>& Other)
	{
		m_p=newgiant(iSize);
		gtog(Other, m_p);
	}

	CUniString ToString(int iBase)
	{
		return gformat(m_p, iBase);
	}

	~CGiantBase()
	{
		free(m_p);
	}

	CGiantBase<iSize>& operator=(const CGiantBase<iSize>& Other)
	{
		gtog(Other, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator+=(const CGiantBase<iSize>& Other)
	{
		addg(Other, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator+=(int i)
	{
		iaddg(i, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator-=(const CGiantBase<iSize>& Other)
	{
		subg(Other, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator-=(int i)
	{
		isubg(i, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator*=(const CGiantBase<iSize>& Other)
	{
		mulg(Other, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator*=(int i)
	{
		imulg(i, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator%=(const CGiantBase<iSize>& Other)
	{
		modg(Other, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator%=(int i)
	{
		imodg(i, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator/=(const CGiantBase<iSize>& Other)
	{
		divg(Other, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator/=(int i)
	{
		idivg(i, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator<<=(int i)
	{
		gshiftleft(i, m_p);
		return *this;
	}

	CGiantBase<iSize>& operator>>=(int i)
	{
		gshiftright(i, m_p);
		return *this;
	}

	CGiantBase<iSize> operator<<(int b)
	{
		CGiantBase<iSize> t(*this);
		t<<=b;
		return t;
	}

	CGiantBase<iSize> operator>>(int b)
	{
		CGiantBase<iSize> t(*this);
		t>>=b;
		return t;
	}

	bool operator!() const
	{
		return !!isZero(m_p);
	}

	operator bool() const
	{
		return (!isZero(m_p));
	}
	operator bool()
	{
		return (!isZero(m_p));
	}


	operator giant()  { return m_p; }
	operator const giant() const { return m_p; }

	int Size() const { return abs(m_p->sign); }

	giant	m_p;
};

template <int iSize>
CGiantBase<iSize> operator+(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	CGiantBase<iSize> t(a);
	t+=b;
	return t;
}

template <int iSize>
CGiantBase<iSize> operator-(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	CGiantBase<iSize> t(a);
	t-=b;
	return t;
}

template <int iSize>
CGiantBase<iSize> operator*(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	CGiantBase<iSize> t(a);
	t*=b;
	return t;
}

template <int iSize>
CGiantBase<iSize> operator/(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	CGiantBase<iSize> t(a);
	t/=b;
	return t;
}

template <int iSize>
CGiantBase<iSize> operator%(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	CGiantBase<iSize> t(a);
	t%=b;
	return t;
}

template <int iSize>
CGiantBase<iSize> absg(const CGiantBase<iSize> a)
{
	CGiantBase<iSize> t(a);
	absg(t);
	return t;
}

template <int iSize>
CGiantBase<iSize> negg(const CGiantBase<iSize> a)
{
	CGiantBase<iSize> t(a);
	negg(t);
	return t;
}


template <int iSize>
CGiantBase<iSize> operator-(const CGiantBase<iSize> a)
{
	CGiantBase<iSize> t(a);
	negg(t);
	return t;
}

template <int iSize>
bool operator<(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	return gcompg(a,b)<0;
}

template <int iSize>
bool operator<=(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	return gcompg(a,b)<=0;
}

template <int iSize>
bool operator>(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	return gcompg(a,b)>0;
}

template <int iSize>
bool operator>=(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	return gcompg(a,b)>=0;
}

template <int iSize>
bool operator==(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	return gcompg(a,b)==0;
}

template <int iSize>
bool operator!=(const CGiantBase<iSize> a, const CGiantBase<iSize> b)
{
	return gcompg(a,b)!=0;
}

}





#endif	// __GIANT_H

