/////////////////////////////////////////////////////////////////////////////
// Sinic.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "Sinic.h"

CCommandLineParser cl;


// Main entry point
int _tmain(int argc, _TCHAR* argv[])
{
	printf("CL:%S\n", GetCommandLine());
	//return 0;

	// Add command line alias'
	cl.AddAlias(L"h", L"?");
	cl.AddAlias(L"help", L"?");
	cl.AddAlias(L"x", L"extract");
	cl.AddAlias(L"o", L"out");

	// Parse command line
	cl.Parse(GetCommandLine(), true);

	// Display logo...
	if (!cl.GetSwitch(L"nologo"))
	{
		wprintf(L"Structured Ini File Compiler v1.1\nCopyright (C) 2007 Topten Software.  All Rights Reserved\n\n");
	}

	// Display help
	if (cl.GetSwitch(L"?"))
	{
		cl.BuildHelp(L"nologo", L"Suppress display of copyright and version info", NULL, clOptional);
		cl.BuildHelp(L"?|h|help", L"Display help information", NULL, clOptional);
		cl.BuildHelp(L"i", L"Include path", L"path", clMulti|clValue|clOptional);
		cl.BuildHelp(L"x|extract", L"Extract (decompile)", NULL, clOptional);
		cl.BuildHelp(L"unicode", L"Decompile to unicode", NULL, clOptional);
		cl.BuildHelp(L"flat", L"Decompile to flat", NULL, clOptional);
		cl.BuildHelp(L"o|out", L"Output File", NULL, clOptional);
		cl.BuildHelp(L"InputFile", L"File to be processed", NULL, clPlaced);
		wprintf(cl.GetHelp(L"Sinic"));
		return 7;
	}

	// Compile or decompile
	bool bDecompile=cl.GetSwitch(L"extract");
	bool bUnicode=cl.GetSwitch(L"unicode");
	bool bFlat=cl.GetSwitch(L"flat");

	// Get input value
	CUniString strInputFile;
	cl.GetPlacedValue(0, L"InputFile", strInputFile);
	strInputFile=QualifyPath(strInputFile);

	// Get output file
	CUniString strOutputFile;
	cl.GetValue(L"out", strOutputFile);
	if (strOutputFile.IsEmpty())
	{
		strOutputFile=ChangeFileExtension(strInputFile.IsEmpty() ? L"" : strInputFile, bDecompile ? L"sini" : L"bini");
	}
	strOutputFile=QualifyPath(strOutputFile);


	// Setup file content provider with include paths
	CFileContentProvider fp;
	CVector<CUniString> vecIncludePaths;
	cl.GetMultiValue(L"i", vecIncludePaths);
	for (int i=0; i<vecIncludePaths.GetSize(); i++)
	{
		CUniString str=QualifyPath(vecIncludePaths[i]);
		fp.AddIncludePath(str);
	}

	// Get defines
	CVector<CUniString> vecDefines;
	cl.GetMultiValue(L"d", vecDefines);

	// Error handling...
	cl.CheckNoUnknownArgs();
	if (cl.HasError())
	{
		wprintf(cl.GetErrorMessage());
		wprintf(L"\n\n");
		return 7;
	}

	if (!bDecompile)
	{
		// Load the input file
		CProfileFile file;
		for (int i=0; i<vecDefines.GetSize(); i++)
		{
			CUniString strName, strValue;
			SplitString(vecDefines[i], L"=", strName, strValue);
			printf("#define %S %S\n", strName, strValue);
			file.Define(strName, strValue);
		}

		if (!file.Load(strInputFile, &fp))
		{
			HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
			wprintf(file.GetParseError());
			return 7;
		}

		// Extract encryption key
		DWORD dwEncryptionKey=0;
		CProfileSection* pSection=file.FindSection(L"Sinic");
		if (pSection)
		{
			// Save the key
			dwEncryptionKey=pSection->GetIntValue(L"EncryptionKey", 0);

			// Remove the "sinic" section
			file.Remove(pSection);
		}

		// Create output file
		CFileStream OutStream;
		if (!OutStream.Create(strOutputFile))
		{
			HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
			wprintf(L"Failed to create '%s' - %s (0x%.8x)\n\n", strOutputFile, FormatError(hr), hr);
			return 7;
		}

		// Save as binary file
		HRESULT hr;
		if (dwEncryptionKey)
		{
			CAutoPtr<IStream, SRefCounted> spEncStream;
			CreateCryptorStream(&OutStream, CCryptorKey(dwEncryptionKey), &spEncStream);
			hr=SaveBinaryProfile(file, spEncStream);
		}
		else
		{
			hr=SaveBinaryProfile(file, &OutStream);
		}
		OutStream.Close();
		if (FAILED(hr))
		{
			HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
			wprintf(L"Failed to save '%s' - %s (0x%.8x)\n\n", strOutputFile, FormatError(hr), hr);
			DeleteFile(strOutputFile);
			return 7;
		}
	}
	else
	{
		// Open stream
		CFileStream InStream;
		if (!InStream.Open(strInputFile))
		{
			HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
			wprintf(L"Failed to open '%s' - %s (0x%.8x)\n\n", strInputFile, FormatError(hr), hr);
			return 7;
		}

		// Load profile
		CProfileFile file;
		HRESULT hr=LoadBinaryProfile(file, &InStream);
		if (FAILED(hr))
		{
			if (hr!=E_UNEXPECTED)
			{
				wprintf(L"Failed to load '%s' - %s (0x%.8x)\n\n", strInputFile, FormatError(hr), hr);
				return 7;
			}
			else
			{
				wprintf(L"Failed to load '%s' - not a binary profile file\n\n", strInputFile);
				return 7;
			}
		}

		// Save as heirarchial
		if (!file.Save(strOutputFile, bUnicode, !bFlat))
		{
			HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
			wprintf(L"Failed to save '%s' - %s (0x%.8x)\n\n", strInputFile, FormatError(hr), hr);
			return 7;
		}
	}

	wprintf(L"%s - OK\n\n", strOutputFile);

	return 0;
}

