// ProfileFileTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	// Create the file
	/*
	{
	CProfileFile file;
	file.Load(L"test.ini");
	file.SetStorageMode(CProfileFile::Binary, L"test.ini");
	file.SetValue(L"Settings", L"String", L"Hello World");
	file.SetIntValue(L"Settings", L"Int", 20);
	file.SetDoubleValue(L"Settings", L"Double", 200.2);

	{
		CComPtr<IStream> spStream;
		file.CreateStream(L"Settings", L"Binary", &spStream);
		byte bData[] = { 1,2,3,4,5,6,7,8,9,10 };
		spStream->Write(bData, sizeof(bData), NULL);
	}

	{
		CComPtr<IStream> spStream;
		file.CreateStream(L"Settings", L"Binary3", &spStream);
		byte bData[] = { 21,22,23,24,25,26,27,28,29,30 };
		spStream->Write(bData, sizeof(bData), NULL);
	}

	//file.Save((wchar_t*)NULL);
	file.Save(L"test.ini");
	}
	*/

	// Open the file
	{
		CProfileFile file;
		file.Load(L"Test.ini");

		CComPtr<IStream> spStream;
		file.OpenStream(L"Settings", L"Binary", &spStream);

		byte bData[100];
		spStream->Read(bData, sizeof(bData), NULL);

		file.SetStorageMode(CProfileFile::Structured);
		file.Save(NULL);
	}


	return 0;
}

