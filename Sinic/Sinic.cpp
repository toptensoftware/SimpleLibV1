/////////////////////////////////////////////////////////////////////////////
// Sinic.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "Sinic.h"

CCommandLineParser cl;

// Main entry point
int _tmain(int argc, _TCHAR* argv[])
{
	// Add command line alias'
	cl.AddAlias(L"h", L"?");
	cl.AddAlias(L"help", L"?");
	cl.AddAlias(L"d", L"decompile");

	// Parse command line
	cl.Parse(GetCommandLine(), true);

	// Display logo...
	if (!cl.GetSwitch(L"nolong"))
	{
		wprintf(L"Structured Ini File Compiler v1.0\nCopyright (C) 2007 Topten Software.  All Rights Reserved\n\n");
	}

	// Display help
	if (cl.GetSwitch(L"?"))
	{
		cl.BuildHelp(L"nologo", L"Suppress display of copyright and version info", NULL, clOptional);
		cl.BuildHelp(L"?|h|help", L"Display help information", NULL, clOptional);
		cl.BuildHelp(L"i", L"Include path", L"path", clMulti|clValue|clOptional);
		cl.BuildHelp(L"d|decompile", L"Decompile", NULL, clOptional);
		cl.BuildHelp(L"unicode", L"Decompile to unicode", NULL, clOptional);
		cl.BuildHelp(L"flat", L"Decompile to flat", NULL, clOptional);
		cl.BuildHelp(L"InputFile", L"File to be processed", NULL, clPlaced);
		cl.BuildHelp(L"OutputFile", L"Output file name", NULL, clPlaced|clOptional);
		wprintf(cl.GetHelp(L"Sinic"));
		return 7;
	}

	// Compile or decompile
	bool bDecompile=cl.GetSwitch(L"decompile");
	bool bUnicode=cl.GetSwitch(L"unicode");
	bool bFlat=cl.GetSwitch(L"flat");

	// Get input value
	CUniString strInputFile;
	cl.GetPlacedValue(0, L"InputFile", strInputFile);
	CUniString strOutputFile;
	cl.GetOptionalPlacedValue(1, L"OutputFile", ChangeFileExtension(strInputFile.IsEmpty() ? L"" : strInputFile, bDecompile ? L"sini" : L"bini"), strOutputFile);

	// Setup file content provider with include paths
	CFileContentProvider fp;
	CVector<CUniString> vecIncludePaths;
	cl.GetMultiValue(L"i", vecIncludePaths);
	for (int i=0; i<vecIncludePaths.GetSize(); i++)
	{
		CUniString str=QualifyPath(vecIncludePaths[i]);
		fp.AddIncludePath(str);
	}

	// Error handling...
	cl.CheckNoUnknownArgs();
	if (cl.HasError())
	{
		wprintf(cl.GetErrorMessage());
		wprintf(L"\n\n");
		return 7;
	}

	// Qualify files
	strInputFile=QualifyPath(strInputFile);
	strOutputFile=QualifyPath(strOutputFile);

	if (!bDecompile)
	{
		// Load the input file
		CProfileFile file;
		if (!file.Load(strInputFile, &fp))
		{
			HRESULT hr=HRESULT_FROM_WIN32(GetLastError());
			wprintf(file.GetParseError());
			return 7;
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
		HRESULT hr=SaveBinaryProfile(file, &OutStream);
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

	wprintf(L"OK\n\n");

	return 0;
}

