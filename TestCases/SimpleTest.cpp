/////////////////////////////////////////////////////////////////////////////
// SimpleTest.cpp : SimpleLib Unit Tests

#include "../SimpleLib.h"

using namespace Simple;

bool g_bAnyFailed=false;
bool g_bFailed=false;


void Failed(int iLine, const char* psz)
{
	if (!g_bFailed) printf("\n");
	g_bAnyFailed=true;
	g_bFailed=true;
	printf("Failed(%i): %s\n", iLine, psz);
}

#undef ASSERT
#define ASSERT(x)  if (!(x)) Failed(__LINE__, #x);

class CInstanceCounter
{
public:
	CInstanceCounter()
	{
		m_iInstances++;
	}

	CInstanceCounter(const CInstanceCounter& Other)
	{
		m_iInstances++;
	}

	virtual ~CInstanceCounter()
	{
		m_iInstances--;
	}

	static int m_iInstances;
};

int CInstanceCounter::m_iInstances=0;

class CMyObject
{
public:
	CMyObject(int iVal) : m_iVal(iVal)
	{
	}
	CMyObject(const CMyObject& Other) : m_iVal(Other.m_iVal)
	{
	}

	int m_iVal;
};

template <>
int Compare(CMyObject* const& a, CMyObject* const& b)
{
	return a->m_iVal-b->m_iVal;
}

template <>
int Compare(const CMyObject& a, const CMyObject& b)
{
	return a.m_iVal-b.m_iVal;
}


void TestStrings()
{
	g_bFailed=false;
	printf("Testing CString...");


	// Basic constructor
	CAnsiString str("Hello");
	ASSERT(Compare(str, "Hello")==0);
	ASSERT(str.GetLength()==5);
	ASSERT(str.Compare("Hello")==0);

	// Append
	str.Append(" World");
	ASSERT(Compare(str, "Hello World")==0);
	ASSERT(str.Compare("Hello World")==0);
	ASSERT(str.GetLength()==11);

	// +=
	str+="!!";
	ASSERT(Compare(str, "Hello World!!")==0);
	ASSERT(str.Compare("Hello World!!")==0);
	ASSERT(str.GetLength()==13);

	// Copy constructor
	CAnsiString str2(str);
	ASSERT(static_cast<const char*>(str)==static_cast<const char*>(str2));			// Pointers should be same

	// Modify string 2, invoking copy on write
	str2+="!";
	ASSERT(Compare(str, "Hello World!!")==0);
	ASSERT(Compare(str2, "Hello World!!!")==0);

	// Character access
	ASSERT(str[0]=='H');
	ASSERT(str[5]==' ');

	// Insert, delete, replace
	str.Insert(5, " there");
	ASSERT(Compare(str, "Hello there World!!")==0);
	str.Delete(5, 6);
	ASSERT(Compare(str, "Hello World!!")==0);
	str.Replace(6, 5, "There");
	ASSERT(Compare(str, "Hello There!!")==0);

	// Large string building
	str2.Empty();
	ASSERT(str2.IsEmpty());
	int i;
	for (i=0; i<10000; i++)
	{
		str2+="**";
	}
	ASSERT(str2.GetLength()==20000);

	// Length specified constructor
	str2=CAnsiString("Hello World", 5);
	ASSERT(Compare(str2, "Hello")==0);

	// Format
	ASSERT(Compare(Format("%s World %i", str2.sz(), 23), "Hello World 23")==0);

	// Unicode Format
	#ifndef SIMPLELIB_NO_VSWPRINTF
	CUniString strU=L"Hello";
	CUniString strR=Format(L"%s World %i", strU.sz(), 24);
//	printf("Result:%S\n", strR.sz());
	ASSERT(Compare(strR, L"Hello World 24")==0);
	#else
	printf("\nUnicode Format() not supported\n");
	#endif

	CAnsiString strA("Hello World");
	ASSERT(strA.Left(5).Compare("Hello")==0);
	ASSERT(strA.Right(5).Compare("World")==0);
	ASSERT(strA.ToUpper().Compare("HELLO WORLD")==0);
	ASSERT(strA.ToLower().Compare("hello world")==0);
	ASSERT(strA.Compare("Hello World")==0);

	CUniString strW(L"Hello World");
	ASSERT(strW.Left(5).Compare(L"Hello")==0);
	ASSERT(strW.Right(5).Compare(L"World")==0);
	ASSERT(strW.ToUpper().Compare(L"HELLO WORLD")==0);
	ASSERT(strW.ToLower().Compare(L"hello world")==0);
	ASSERT(strW.Compare(L"Hello World")==0);

	// Left, Right, Mid
	ASSERT(Compare(Left("Hello World",5), "Hello")==0);
	ASSERT(Compare(Right("Hello World", 5), "World")==0);
	ASSERT(Compare(Mid("Hello World", 1, 4), "ello")==0);
	ASSERT(Compare(Mid("Hello World", 6, -1), "World")==0);
	ASSERT(Compare(Mid("Hello World", -5, -1), "World")==0);

	ASSERT(CompareI("Hello World", "hello world")==0);
	ASSERT(CompareI(L"Hello World", L"hello world")==0);

	// Ansi <-> Unicode type conversions
	ASSERT(Compare(L"Hello World", a2w("Hello World"))==0);
	ASSERT(Compare("Hello World", w2a(L"Hello World"))==0);

	CAnsiString strAnsi(w2a(L"Hello World"));
	ASSERT(Compare(strAnsi, "Hello World")==0);

	CUniString strUnicode(a2w("Hello World"));
	ASSERT(Compare(strUnicode, L"Hello World")==0);

	if (!g_bFailed)
		printf("OK\n");
}

int CompareIntDouble(const int& iVal, double dbl)
{
	return iVal-int(dbl);
}

int CompareIntKey(const CMyObject& p, int iVal)
{
	return p.m_iVal-iVal;
}

int CompareIntKeyPtr(CMyObject* const& p, int iVal)
{
	return p->m_iVal-iVal;
}

void TestVector()
{
	g_bFailed=false;
	printf("Testing CVector...");

	// Setup a vector
	CVector<int> vec;

	// Add 10 items
	int i;
	for (i=0; i<10; i++)
	{
		vec.Add(i);
	}

	/*
	int iVal;
	vec.Pop(iVal);
	iVal++;
	vec.Push(iVal);
	*/

	// Test initial addd
	ASSERT(vec.GetSize()==10);
	for (i=0; i<10; i++)
	{
		ASSERT(vec[i]==i);
	}

	// Insert At
	vec.InsertAt(5, 100);
	ASSERT(vec[4]==4);
	ASSERT(vec[5]==100);
	ASSERT(vec[6]==5);

	// Remove At
	vec.RemoveAt(5);
	ASSERT(vec[4]==4);
	ASSERT(vec[5]==5);

	// Remove At (multiple)
	vec.InsertAt(5, 100);
	vec.InsertAt(5, 100);
	vec.InsertAt(5, 100);
	vec.InsertAt(5, 100);
	vec.RemoveAt(5, 4);
	ASSERT(vec[4]==4);
	ASSERT(vec[5]==5);

	// Swap
	vec.Swap(1,2);
	ASSERT(vec[0]==0);
	ASSERT(vec[1]==2);
	ASSERT(vec[2]==1);
	ASSERT(vec[3]==3);
	vec.Swap(1,2);

	// Move
	vec.Move(3,1);
	ASSERT(vec[0]==0);
	ASSERT(vec[1]==3);
	ASSERT(vec[2]==1);
	ASSERT(vec[3]==2);

	// Move back
	vec.Move(1,3);
	ASSERT(vec[0]==0);
	ASSERT(vec[1]==1);
	ASSERT(vec[2]==2);
	ASSERT(vec[3]==3);

	// Find
	ASSERT(vec.Find(100)==-1);
	ASSERT(vec.Find(8)==8);

	// Find with start position
	vec.Add(1);
	ASSERT(vec.Find(1)==1);
	ASSERT(vec.Find(1,5)==10);
	ASSERT(!vec.IsEmpty());

	// Remove All
	vec.RemoveAll();
	ASSERT(vec.GetSize()==0);
	ASSERT(vec.IsEmpty());

	// Quick sort
	for (i=0; i<100; i++)
	{
		vec.Add(rand());
	}
	vec.QuickSort();

	// Check order and remove duplicates
	for (i=vec.GetSize()-1; i>0; i--)
	{
		if (vec[i]==vec[i-1])
		{
			vec.RemoveAt(i);
		}
		else
		{
			ASSERT(vec[i]>=vec[i-1]);
		}
	}

	// Quick search
	for (i=0; i<vec.GetSize(); i++)
	{
		 int iPos;
		 ASSERT(vec.QuickSearch(vec[i], iPos)==true);
		 ASSERT(iPos==i);
	}

	// Quick search insert position test...
	vec.RemoveAll();
	for (i=1; i<100; i++)
	{
		vec.Add(i*10);
	}
	int iPos;
	ASSERT(vec.QuickSearch(5, iPos)==false && iPos==0);
	ASSERT(vec.QuickSearch(15, iPos)==false && iPos==1);
	ASSERT(vec.QuickSearch(20, iPos)==true && iPos==1);
	ASSERT(vec.QuickSearch(25, iPos)==false && iPos==2);
	ASSERT(vec.QuickSearch(10001, iPos)==false && iPos==vec.GetSize());

	ASSERT(vec.FindKey(12.0, CompareIntDouble)==-1);
	ASSERT(vec.FindKey(10.0, CompareIntDouble)==0);
	ASSERT(vec.FindKey(20.0, CompareIntDouble)==1);

	/*
	ASSERT(Find(vec, 12.0, CompareIntDouble)==-1);
	ASSERT(Find(vec, 10.0, CompareIntDouble)==0);
	ASSERT(Find(vec, 20.0, CompareIntDouble)==1);
	*/

	ASSERT(vec.QuickSearchKey(20.0, CompareIntDouble, iPos)==true && iPos==1);

	{
	CVector<CMyObject> vecMyObjs;
	for (i=0; i<100; i++)
		vecMyObjs.Add(CMyObject(i*10));
	vecMyObjs.QuickSort();

	ASSERT(vecMyObjs.QuickSearchKey(20, CompareIntKey, iPos)==true && iPos==2);
	}

	{
	CVector<CMyObject*, SOwnedPtr> vecMyObjs;
	for (i=0; i<100; i++)
		vecMyObjs.Add(new CMyObject(i*10));
	vecMyObjs.QuickSort();

	ASSERT(vecMyObjs.QuickSearchKey(20, CompareIntKeyPtr, iPos)==true && iPos==2);
	}


	// Test stack operations
	vec.RemoveAll();
	vec.Push(10);
	vec.Push(20);
	vec.Push(30);
	ASSERT(vec[0]==10);
	ASSERT(vec[1]==20);
	ASSERT(vec[2]==30);
	ASSERT(vec.Top()==30);
	ASSERT(vec.Pop()==30);
	ASSERT(vec.Pop()==20);
	ASSERT(vec.Pop()==10);
	ASSERT(vec.IsEmpty());

	// Test queue operations
	vec.RemoveAll();
	vec.Enqueue(10);
	vec.Enqueue(20);
	vec.Enqueue(30);
	ASSERT(vec[0]==10);
	ASSERT(vec[1]==20);
	ASSERT(vec[2]==30);
	ASSERT(vec.Peek()==10);
	ASSERT(vec.Dequeue()==10);
	ASSERT(vec.Dequeue()==20);
	ASSERT(vec.Dequeue()==30);
	ASSERT(vec.IsEmpty());

	// Test construction/destruction when holding objects
	CVector<CInstanceCounter>	vecObjs;
	{
	vecObjs.Add(CInstanceCounter());
	vecObjs.Add(CInstanceCounter());
	vecObjs.Add(CInstanceCounter());
	}
	ASSERT(CInstanceCounter::m_iInstances==3);
	vecObjs.RemoveAt(0);
	ASSERT(CInstanceCounter::m_iInstances==2);
	vecObjs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==0);


	// Test construction/destruction when holding owned object pointers
	CVector<CInstanceCounter*, SOwnedPtr> vecPtrs;
	vecPtrs.Add(new CInstanceCounter());
	vecPtrs.Add(new CInstanceCounter());
	vecPtrs.Add(new CInstanceCounter());
	ASSERT(CInstanceCounter::m_iInstances==3);
	vecPtrs.RemoveAt(0);
	ASSERT(CInstanceCounter::m_iInstances==2);
	CInstanceCounter* pDetach=vecPtrs.DetachAt(0);
	ASSERT(CInstanceCounter::m_iInstances==2);
	vecPtrs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==1);
	delete pDetach;
	ASSERT(CInstanceCounter::m_iInstances==0);


	if (!g_bFailed)
		printf("OK\n");
}

void TestSortedVector()
{
	printf("Testing CSortedVector...");
	g_bFailed=false;

	CSortedVector<CUniString>	vec;

	vec.Add(L"Apples");
	vec.Add(L"Pears");
	vec.Add(L"Bananas");
	vec.Add(L"apples");
	vec.Add(L"pears");
	vec.Add(L"bananas");

	for (int i=1; i<vec.GetSize(); i++)
	{
		ASSERT(wcscmp(vec[i], vec[i-1])>0);
	}

	vec.Resort(CompareI);

	for (int i=1; i<vec.GetSize(); i++)
	{
		const wchar_t* psz1=vec[i-1];
		const wchar_t* psz2=vec[i];
		ASSERT(_wcsicmp(psz2, psz1)>=0);
	}

	if (!g_bFailed)
		printf("OK\n");
}

void TestMap()
{
	printf("Testing CMap...");
	g_bFailed=false;

	// Start with empty map
	CMap<int, int> map;
	ASSERT(map.IsEmpty());
	ASSERT(map.GetSize()==0);

	// Add some items
	int i;
	for (i=1; i<=100; i++)
	{
		map.Add(i, i*10);
	}
	ASSERT(!map.IsEmpty());
	ASSERT(map.GetSize()==100);
#ifdef _DEBUG
	map.CheckAll();
#endif


	// Check lookups...
	ASSERT(map.Get(1)==10);
	ASSERT(map.Get(5)==50);
	ASSERT(map.Get(10)==100);
	ASSERT(map.Get(2000, -1)==-1);
	ASSERT(map.HasKey(1));
	ASSERT(map.HasKey(50));
	ASSERT(map.HasKey(100));
	ASSERT(!map.HasKey(0));
	ASSERT(!map.HasKey(101));

	// Check find  (Get above uses Find so just one test...)
	int iValue;
	ASSERT(map.Find(1, iValue)==true && iValue==10);

	// Test iteration
	for (i=0; i<map.GetSize(); i++)
	{
		ASSERT(map[i].Key==i+1);
		ASSERT(map[i].Value==map[i].Key*10);
	}
#ifdef _DEBUG
	map.CheckAll();
#endif

	// Test iteration while removing... remove all odd elements
	// NB: Behaviour when forward iterating and removing should be same as for
	//		vector, where when removing current element, iteration index should be decremented
	int iTotal=map.GetSize();
	int iIndex=0;
	for (i=0; i<map.GetSize(); i++, iIndex++)
	{
		int iKey=map[i].Key;
		ASSERT(iKey==iIndex+1);

		if ((iKey%2)!=0)
		{
			map.Remove(map[i].Key);
			iTotal--;
			i--;
		}
	}
#ifdef _DEBUG
	map.CheckAll();
#endif
	ASSERT(map.GetSize()==iTotal);

	// Check only evens left...
	ASSERT(map.GetSize()==50);
	for (i=0; i<map.GetSize(); i++)
	{
		ASSERT((map[i].Key%2)==0);
	}

	// Test reverse iteration while removing
	for (i=map.GetSize()-1; i>=0; i--)
	{
		if ((map[i].Key % 10)==0)
		{
			map.Remove(map[i].Key);
			iTotal--;
		}
	}
#ifdef _DEBUG
	map.CheckAll();
#endif
	ASSERT(map.GetSize()==iTotal);

	// Repopulate...
	map.RemoveAll();
	ASSERT(map.GetSize()==0);
	ASSERT(map.IsEmpty());
	for (i=0; i<100; i++)
	{
		map.Add(i*10, i*10);
	}

//	srand(0);

	// Test insertion while iterating...
	int iExpectedLoopCount=map.GetSize();
	int iLoopCount=0;
	int iPrevKey=0;
	for (i=0; i<map.GetSize(); i++)
	{
		int iKey=map[i].Key;

		if (i>0)
		{
			ASSERT(iKey>iPrevKey);
		}
		iPrevKey=iKey;


		ASSERT(map[i].Key==map[i].Value);

		iLoopCount++;

		int iNew=rand()%1000;
		if (iNew<=iKey)
		{
			if (!map.HasKey(iNew))
				i++;
		}

		if (iNew>iKey)
		{
			if (!map.HasKey(iNew))
				iExpectedLoopCount++;
		}

		map.Add(iNew, iNew);
	}
	ASSERT(iExpectedLoopCount==iLoopCount);


	// Test construction/destruction when holding objects
	// NB: There's an extra 1 on the reference count because of the instance in
	//		the m_Leaf member of the map itself.
	CMap<int, CInstanceCounter>	mapObjs;
	{ mapObjs.Add(10, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==2);
	{ mapObjs.Add(20, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==3);
	{ mapObjs.Add(30, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==4);
	mapObjs.Remove(10);
	ASSERT(CInstanceCounter::m_iInstances==3);
	mapObjs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==1);


	// Test construction/destruction when holding owned object pointers
	CMap<int, CInstanceCounter*, SValue, SOwnedPtr> mapPtrs;
	mapPtrs.Add(10, new CInstanceCounter());
	mapPtrs.Add(20, new CInstanceCounter());
	mapPtrs.Add(30, new CInstanceCounter());
	ASSERT(CInstanceCounter::m_iInstances==4);
	mapPtrs.Remove(10);
	ASSERT(CInstanceCounter::m_iInstances==3);
	CInstanceCounter* pDetach=mapPtrs.Detach(20);
	ASSERT(CInstanceCounter::m_iInstances==3);
	mapPtrs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==2);
	delete pDetach;
	ASSERT(CInstanceCounter::m_iInstances==1);

	if (!g_bFailed)
		printf("OK\n");
}

void TestIndex()
{
	printf("Testing CIndex...");
	g_bFailed=false;

	// Start with empty map
	CIndex<int, int> map;
	ASSERT(map.IsEmpty());
	ASSERT(map.GetSize()==0);

	// Add some items
	int i;
	for (i=1; i<=100; i++)
	{
		map.Add(i, i*10);
	}
	ASSERT(!map.IsEmpty());
	ASSERT(map.GetSize()==100);


	// Check lookups...
	ASSERT(map.Get(1)==10);
	ASSERT(map.Get(5)==50);
	ASSERT(map.Get(10)==100);
	ASSERT(map.Get(2000, -1)==-1);
	ASSERT(map.HasKey(1));
	ASSERT(map.HasKey(50));
	ASSERT(map.HasKey(100));
	ASSERT(!map.HasKey(0));
	ASSERT(!map.HasKey(101));

	// Check find  (Get above uses Find so just one test...)
	int iValue;
	ASSERT(map.Find(1, iValue)==true && iValue==10);

	// Test iteration
	for (i=0; i<map.GetSize(); i++)
	{
		ASSERT(map[i].Key==i+1);
		ASSERT(map[i].Value==map[i].Key*10);
	}

	// Test iteration while removing... remove all odd elements
	// NB: Behaviour when forward iterating and removing should be same as for
	//		vector, where when removing current element, iteration index should be decremented
	int iTotal=map.GetSize();
	int iIndex=0;
	for (i=0; i<map.GetSize(); i++, iIndex++)
	{
		int iKey=map[i].Key;
		ASSERT(iKey==iIndex+1);

		if ((iKey%2)!=0)
		{
			map.Remove(map[i].Key);
			iTotal--;
			i--;
		}
	}
	ASSERT(map.GetSize()==iTotal);

	// Check only evens left...
	ASSERT(map.GetSize()==50);
	for (i=0; i<map.GetSize(); i++)
	{
		ASSERT((map[i].Key%2)==0);
	}

	// Test reverse iteration while removing
	for (i=map.GetSize()-1; i>=0; i--)
	{
		if ((map[i].Key % 10)==0)
		{
			map.Remove(map[i].Key);
			iTotal--;
		}
	}
	ASSERT(map.GetSize()==iTotal);

	// Repopulate...
	map.RemoveAll();
	ASSERT(map.GetSize()==0);
	ASSERT(map.IsEmpty());
	for (i=0; i<100; i++)
	{
		map.Add(i*10, i*10);
	}

	// Test insertion while iterating...
	int iExpectedLoopCount=map.GetSize();
	int iLoopCount=0;
	int iPrevKey=0;
	for (i=0; i<map.GetSize(); i++)
	{
		int iKey=map[i].Key;

		if (i>0)
		{
			ASSERT(iKey>iPrevKey);
		}
		iPrevKey=iKey;


		ASSERT(map[i].Key==map[i].Value);

		iLoopCount++;

		int iNew=rand()%1000;
		if (iNew<iKey)
		{
			if (!map.HasKey(iNew))
				i++;
		}

		if (iNew>iKey)
		{
			if (!map.HasKey(iNew))
				iExpectedLoopCount++;
		}

		map.Add(iNew, iNew);
	}
	ASSERT(iExpectedLoopCount==iLoopCount);


	// Test construction/destruction when holding objects
	CIndex<int, CInstanceCounter>	mapObjs;
	{ mapObjs.Add(10, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==1);
	{ mapObjs.Add(20, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==2);
	{ mapObjs.Add(30, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==3);
	mapObjs.Remove(10);
	ASSERT(CInstanceCounter::m_iInstances==2);
	mapObjs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==0);


	// Test construction/destruction when holding owned object pointers
	CIndex<int, CInstanceCounter*, SValue, SOwnedPtr> mapPtrs;
	mapPtrs.Add(10, new CInstanceCounter());
	mapPtrs.Add(20, new CInstanceCounter());
	mapPtrs.Add(30, new CInstanceCounter());
	ASSERT(CInstanceCounter::m_iInstances==3);
	mapPtrs.Remove(10);
	ASSERT(CInstanceCounter::m_iInstances==2);
	CInstanceCounter* pDetach=mapPtrs.Detach(20);
	ASSERT(CInstanceCounter::m_iInstances==2);
	mapPtrs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==1);
	delete pDetach;
	ASSERT(CInstanceCounter::m_iInstances==0);

	if (!g_bFailed)
		printf("OK\n");
}

void TestHashMap()
{
	printf("Testing CHashMap...");
	g_bFailed=false;

	// Start with empty map
	CHashMap<int, int> map;
	ASSERT(map.IsEmpty());
	ASSERT(map.GetSize()==0);

	// Add some items
	int i;
	for (i=1; i<=100; i++)
	{
		map.Add(i, i*10);
	}
	ASSERT(!map.IsEmpty());
	ASSERT(map.GetSize()==100);


	// Check lookups...
	ASSERT(map.Get(1)==10);
	ASSERT(map.Get(5)==50);
	ASSERT(map.Get(10)==100);
	ASSERT(map.Get(2000, -1)==-1);
	ASSERT(map.HasKey(1));
	ASSERT(map.HasKey(50));
	ASSERT(map.HasKey(100));
	ASSERT(!map.HasKey(0));
	ASSERT(!map.HasKey(101));

	// Check find  (Get above uses Find so just one test...)
	int iValue;
	ASSERT(map.Find(1, iValue)==true && iValue==10);

	// Test iteration
	for (i=0; i<map.GetSize(); i++)
	{
		ASSERT(map[i].Key==i+1);
		ASSERT(map[i].Value==map[i].Key*10);
	}

	// Test iteration while removing... remove all odd elements
	// NB: Behaviour when forward iterating and removing should be same as for
	//		vector, where when removing current element, iteration index should be decremented
	int iTotal=map.GetSize();
	int iIndex=0;
	for (i=0; i<map.GetSize(); i++, iIndex++)
	{
		int iKey=map[i].Key;
		ASSERT(iKey==iIndex+1);

		if ((iKey%2)!=0)
		{
			map.Remove(map[i].Key);
			iTotal--;
			i--;
		}
	}
	ASSERT(map.GetSize()==iTotal);

	// Check only evens left...
	ASSERT(map.GetSize()==50);
	for (i=0; i<map.GetSize(); i++)
	{
		ASSERT((map[i].Key%2)==0);
	}

	// Test reverse iteration while removing
	for (i=map.GetSize()-1; i>=0; i--)
	{
		if ((map[i].Key % 10)==0)
		{
			map.Remove(map[i].Key);
			iTotal--;
		}
	}
	ASSERT(map.GetSize()==iTotal);

	// Repopulate...
	map.RemoveAll();
	ASSERT(map.GetSize()==0);
	ASSERT(map.IsEmpty());
	for (i=0; i<100; i++)
	{
		map.Add(i*10, i*10);
	}

	// Test insertion while iterating...
	/*
	int iExpectedLoopCount=map.GetSize();
	int iLoopCount=0;
	int iPrevKey=0;
	for (i=0; i<map.GetSize(); i++)
	{
		int iKey=map[i].Key;

		if (i>0)
		{
			ASSERT(iKey>iPrevKey);
		}
		iPrevKey=iKey;


		ASSERT(map[i].Key==map[i].Value);

		iLoopCount++;

		int iNew=rand()%1000;
		if (iNew<iKey)
		{
			if (!map.HasKey(iNew))
				i++;
		}

		if (iNew>iKey)
		{
			if (!map.HasKey(iNew))
				iExpectedLoopCount++;
		}

		map.Add(iNew, iNew);
	}
	ASSERT(iExpectedLoopCount==iLoopCount);
	*/


	// Test construction/destruction when holding objects
	CHashMap<int, CInstanceCounter>	mapObjs;
	{ mapObjs.Add(10, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==1);
	{ mapObjs.Add(20, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==2);
	{ mapObjs.Add(30, CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==3);
	mapObjs.Remove(10);
	ASSERT(CInstanceCounter::m_iInstances==2);
	mapObjs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==0);


	// Test construction/destruction when holding owned object pointers
	CHashMap<int, CInstanceCounter*, SValue, SOwnedPtr> mapPtrs;
	mapPtrs.Add(10, new CInstanceCounter());
	mapPtrs.Add(20, new CInstanceCounter());
	mapPtrs.Add(30, new CInstanceCounter());
	ASSERT(CInstanceCounter::m_iInstances==3);
	mapPtrs.Remove(10);
	ASSERT(CInstanceCounter::m_iInstances==2);
	CInstanceCounter* pDetach=mapPtrs.Detach(20);
	ASSERT(CInstanceCounter::m_iInstances==2);
	mapPtrs.RemoveAll();
	ASSERT(CInstanceCounter::m_iInstances==1);
	delete pDetach;
	ASSERT(CInstanceCounter::m_iInstances==0);

	if (!g_bFailed)
		printf("OK\n");
}

void TestRingBuffer()
{
	printf("Testing CRingBuffer...");
	g_bFailed=false;

	// Create a ring buffer
	CRingBuffer<int> buf(100);

	// Get close to wrap point to make tests below more significant
	int i;
	for (i=0; i<98; i++)
	{
		buf.Enqueue(i);
		buf.Dequeue();
	}

	ASSERT(buf.IsEmpty());
	ASSERT(!buf.IsFull());
	ASSERT(buf.GetSize()==0);

	// Enqueue some stuff
	buf.Enqueue(10);
	buf.Enqueue(20);
	buf.Enqueue(30);
	ASSERT(buf.GetSize()==3);
	buf.Enqueue(40);
	ASSERT(buf.GetSize()==4);
	ASSERT(buf.Unenqueue()==40);
	ASSERT(buf.GetSize()==3);

	// Peek
	int iVal;
	ASSERT(buf.Peek(iVal)==true && iVal==10);
	ASSERT(buf.Peek()==10);

	// Peek Last
	ASSERT(buf.PeekLast()==30);
	ASSERT(buf.PeekLast(iVal)==true && iVal==30);

	// Dequeue
	ASSERT(buf.Dequeue(iVal)==true && iVal==10);
	ASSERT(buf[0]==20);
	ASSERT(buf[1]==30);
	ASSERT(buf.GetSize()==2);
	ASSERT(buf.Dequeue()==20);
	ASSERT(buf.Dequeue()==30);
	ASSERT(buf.IsEmpty());

	// Enqueue and fill buffer
	for (i=0; i<buf.GetCapacity(); i++)
	{
		buf.Enqueue(i);
	}
	ASSERT(buf.IsFull());
	ASSERT(buf.GetSize()==buf.GetCapacity());
	ASSERT(!buf.IsOverflow());

	// Test overflow
	buf.Enqueue(1);
	ASSERT(buf.IsOverflow());
	ASSERT(buf.GetSize()==buf.GetCapacity());

	// Test reset after overflow
	buf.RemoveAll();
	ASSERT(!buf.IsOverflow());
	ASSERT(buf.GetSize()==0);
	ASSERT(buf.IsEmpty());

	buf.Reset(3);
	buf.Enqueue(10);
	buf.Enqueue(20);
	buf.Enqueue(30);


	CRingBuffer<CInstanceCounter> vecObjs(10);
	{
	vecObjs.Enqueue(CInstanceCounter());
	vecObjs.Enqueue(CInstanceCounter());
	vecObjs.Enqueue(CInstanceCounter());
	}
	ASSERT(CInstanceCounter::m_iInstances==3);

	{ vecObjs.Enqueue(CInstanceCounter()); }
	ASSERT(CInstanceCounter::m_iInstances==4);
	vecObjs.Unenqueue();
	ASSERT(CInstanceCounter::m_iInstances==3);

	vecObjs.Dequeue();
	ASSERT(CInstanceCounter::m_iInstances==2);
	vecObjs.Dequeue();
	vecObjs.Dequeue();
	ASSERT(CInstanceCounter::m_iInstances==0);



	// Done
	if (!g_bFailed)
		printf("OK\n");
}



class CItem
{
public:
	CItem(int iVal) : m_iVal(iVal)
	{
		m_iCount++;
	};

	virtual ~CItem()
	{
		m_iCount--;
	}

	int				m_iVal;
	CChain<CItem>	m_Chain;
#ifndef _SIMPLELIB_NO_LINKEDLIST_MULTICHAIN
	CChain<CItem>	m_Chain1;
	CChain<CItem>	m_Chain2;
#endif
	static int		m_iCount;
};

int CItem::m_iCount=0;

void TestLinkedList()
{
	printf("Testing CLinkedList...");
	g_bFailed=false;

	CLinkedList<CItem, SOwnedPtr>	List;

	int i;
	for (i=0; i<10; i++)
		List.Add(new CItem(i));

	ASSERT(CItem::m_iCount==10);

	int iVal=0;
	for (List.MoveFirst(); !List.IsEOF(); List.MoveNext())
	{
		ASSERT(List.Current()->m_iVal==iVal);
		iVal++;
	}

	iVal=9;
	for (List.MoveLast(); !List.IsBOF(); List.MovePrevious())
	{
		ASSERT(List.Current()->m_iVal==iVal);
		iVal--;
	}

	// Delete while iterating forwrad
	for (List.MoveFirst(); !List.IsEOF(); List.MoveNext())
	{
		if ((List.Current()->m_iVal % 2)==0)
			List.Remove(List.Current());
	}
	ASSERT(List.GetSize()==5);
	ASSERT(CItem::m_iCount==5);

	// Clean up
	List.RemoveAll();
	ASSERT(List.IsEmpty());
	ASSERT(List.GetSize()==0);
	ASSERT(CItem::m_iCount==0);


	// Repopulate
	for (i=0; i<10; i++)
		List.Add(new CItem(i));

	// Delete while iterating forwrad
	for (List.MoveLast(); !List.IsBOF(); List.MovePrevious())
	{
		if ((List.Current()->m_iVal % 2)==0)
			List.Remove(List.Current());
	}
	ASSERT(List.GetSize()==5);
	ASSERT(CItem::m_iCount==5);

	// Test detach
	CItem* pItem=List.Detach(List.GetFirst());
	ASSERT(List.GetSize()==4);
	ASSERT(CItem::m_iCount==5);
	delete pItem;
	ASSERT(CItem::m_iCount==4);

	// Repopulate
	List.RemoveAll();
	for (i=0; i<10; i++)
		List.Add(new CItem(i));

#ifndef _SIMPLELIB_NO_LINKEDLIST_MULTICHAIN
	// Multiple lists
	CLinkedList<CItem, SValue, &CItem::m_Chain1>	List1;
	CLinkedList<CItem, SValue, &CItem::m_Chain2>	List2;

	for (List.MoveFirst(); !List.IsEOF(); List.MoveNext())
	{
		List1.Add(List.Current());
		List2.Insert(List.Current(), List2.GetFirst());
	}

	iVal=0;
	for (List1.MoveFirst(); !List1.IsEOF(); List1.MoveNext())
	{
		ASSERT(List1.Current()->m_iVal==iVal);
		iVal++;
	}

	iVal=9;
	for (List2.MoveFirst(); !List2.IsEOF(); List2.MoveNext())
	{
		ASSERT(List2.Current()->m_iVal==iVal);
		iVal--;
	}

	ASSERT(List1.GetSize()==10);
	ASSERT(List2.GetSize()==10);

	List1.RemoveAll();
	List2.RemoveAll();
#endif


	// Test pseudo random access...
	for (i=0; i<List.GetSize(); i++)
	{
		CItem* pItem=List[i];
		ASSERT(pItem->m_iVal==i);
	}
	ASSERT(List[3]->m_iVal==3);
	ASSERT(List[5]->m_iVal==5);
	ASSERT(List[9]->m_iVal==9);

	// Test insert...
	/*
	List.Insert(new CItem(100), List[5]);
	ASSERT(List.GetSize()==11);
	ASSERT(List[4]->m_iVal==4);
	ASSERT(List[5]->m_iVal==100);
	ASSERT(List[6]->m_iVal==5);
	*/


	// Populate 100 items
	List.RemoveAll();
	for (i=0; i<100; i++)
	{
		List.Add(new CItem(i));
	}
	// Test iteration while removing... remove all odd elements
	// NB: Behaviour when forward iterating and removing should be same as for
	//		vector, where when removing current element, iteration index should be decremented
	int iTotal=List.GetSize();
	int iIndex=0;
	for (i=0; i<List.GetSize(); i++, iIndex++)
	{
		int iKey=List[i]->m_iVal;
		ASSERT(iKey==iIndex);

		if ((iKey%2)!=0)
		{
			List.Remove(List[i]);
			iTotal--;
			i--;
		}
	}
	ASSERT(List.GetSize()==iTotal);

	// Check only evens left...
	ASSERT(List.GetSize()==50);
	for (i=0; i<List.GetSize(); i++)
	{
		ASSERT((List[i]->m_iVal%2)==0);
	}

	// Test reverse iteration while removing
	for (i=List.GetSize()-1; i>=0; i--)
	{
		if ((List[i]->m_iVal % 10)==0)
		{
			List.Remove(List[i]);
			iTotal--;
		}
	}
	ASSERT(List.GetSize()==iTotal);


	// Setup list and vector with same content....
	List.RemoveAll();
	CVector<int> vec;
	for (i=0; i<10; i++)
	{
		vec.Add((i+1)*100);
		List.Add(new CItem((i+1)*100));
	}


	//printf("\n");


	// Perform random inserts while iterating...
	srand(0);
	for (i=0; i<List.GetSize(); i+=2)
	{
		int iInsertPos=rand() % List.GetSize();
		int iVal=List[i]->m_iVal*2;
		ASSERT(vec[i]==List[i]->m_iVal);
		List.Insert(new CItem(iVal), List[iInsertPos]);
		vec.InsertAt(iInsertPos, iVal);

		/*
		printf("vec :");
		for (int j=0; j<vec.GetSize(); j++)
			printf("%i ", vec[j]);
		printf("\n");

		printf("list:");
		for (List.MoveFirst(); !List.IsEOF(); List.MoveNext())
			printf("%i ", List.Current()->m_iVal);
		printf("\n");
		*/
	}

	// Check list and vector match...
	ASSERT(List.GetSize()==vec.GetSize());
	for (i=0; i<vec.GetSize(); i++)
	{
		ASSERT(List[i]->m_iVal==vec[i]);
	}




	// Done
	if (!g_bFailed)
		printf("OK\n");
}




class CSingleObj : public CSingleton<CSingleObj>
{
public:
			CSingleObj() {};
	virtual ~CSingleObj() {};
};

void TestSingleton()
{
	printf("Testing CSingleton...");
	g_bFailed=false;


	// Test NULL instance
	ASSERT(CSingleObj::GetInstance()==NULL);

	{
		// Test instance created
		CSingleObj obj;
		ASSERT(CSingleObj::GetInstance()==&obj);
	}

	// Test instance gone...
	ASSERT(CSingleObj::GetInstance()==NULL);

	// Done
	if (!g_bFailed)
		printf("OK\n");
}



#ifndef _SIMPLELIB_NO_DYNAMIC
class CFruit : public CDynamic<CFruit>
{
	virtual void x()=0;
public:
	static const wchar_t* GetTypeName() { return L"CFruit"; }
};

class CApple : public CDynamic<CApple, CFruit>
{
public:
	static const wchar_t* GetTypeName() { return L"CApple"; }
};

class CRedApple : public CDynamicCreatable<CRedApple, CApple>
{
	virtual void x() {};
public:
	static int GenerateTypeID() { return 1; };
	static const wchar_t* GetTypeName() { return L"CRedApple"; }
};

class CGreenApple : public CDynamicCreatable<CGreenApple, CApple, 2>
{
	virtual void x() {};
public:
	static const wchar_t* GetTypeName() { return L"CGreenApple"; }
};

class CBanana : public CDynamicCreatable<CBanana, CFruit, 3>
{
	virtual void x() {};
public:
	static const wchar_t* GetTypeName() { return L"CBanana"; }
};
#endif



void TestDynamic()
{
	printf("Testing CDynamic...");

#ifdef _SIMPLELIB_NO_DYNAMIC
	printf("Skipped - not supported\n");
#else
	g_bFailed=false;

	CApple* p=new CRedApple();
	CFruit* pFruit=p;

	ASSERT(p->QueryAs<CApple>()!=NULL);
	ASSERT(p->QueryAs<CRedApple>()!=NULL);
	ASSERT(p->QueryAs<CGreenApple>()==NULL);
	ASSERT(p->QueryAs<CBanana>()==NULL);

	pFruit=(CFruit*)CRedApple::GetType()->CreateInstance();
	ASSERT(pFruit->QueryAs<CApple>()!=NULL);
	delete pFruit;


	pFruit=(CFruit*)CDynType::GetTypeFromName(L"CRedApple")->CreateInstance();
	ASSERT(pFruit->QueryAs<CApple>()!=NULL);
	ASSERT(pFruit->QueryType()==CRedApple::GetType());
	ASSERT(pFruit->QueryType()->GetID()==1);

	delete pFruit;

	if (!g_bFailed)
		printf("OK\n");
#endif
}

void TestUniStringVector()
{
	printf("Testing CUniStringVector...");

	CUniStringVector vec;
	vec.Add(L"Apples");
	vec.Add(L"Pears");
	vec.Add(L"bananas");

	vec.SortInsensitive();
	ASSERT(IsEqualString(vec[0], L"Apples"));
	ASSERT(IsEqualString(vec[1], L"bananas"));
	ASSERT(IsEqualString(vec[2], L"Pears"));
	vec.Sort();
	ASSERT(IsEqualString(vec[0], L"Apples"));
	ASSERT(IsEqualString(vec[1], L"Pears"));
	ASSERT(IsEqualString(vec[2], L"bananas"));

	ASSERT(vec.FindInsensitive(L"Apples")==0);
	ASSERT(vec.FindInsensitive(L"apples")==0);
	ASSERT(vec.Find(L"Apples")==0);
	ASSERT(vec.Find(L"apples")==-1);

	if (!g_bFailed)
		printf("OK\n");
}


// Main entry point
int main(int argc, char* argv[])
{
	printf("SimpleLib Unit Test Cases\n");

	TestStrings();
	TestVector();
	TestSortedVector();
	TestMap();
	TestIndex();
	TestHashMap();
	TestLinkedList();
	TestRingBuffer();
	TestSingleton();
	TestDynamic();
	TestUniStringVector();

	if (g_bAnyFailed)
	{
		printf("Finished - errors found.\n\n");
	}
	else
	{
		printf("Finished - all tests passed.\n\n");
	}

	return 0;
}

