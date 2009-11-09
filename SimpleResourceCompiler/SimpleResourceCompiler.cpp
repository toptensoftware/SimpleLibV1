// SimpleResourceCompiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void ShowHelp()
{
	printf("Simple Resource Compiler v1.0\n");
	printf("Copyright © 2009 Topten Software. All Rights Reserved\n\n");
	printf("Usage:\tsrc [-?] [-i:<includepath>] [-d:<define>] [-o:<outputfile>] [-dump] [-decompile] [@<responsefile>] file \n\n");
	printf("    file           input file\n");
	printf("    -i:<path>      add include path\n");
	printf("    -d:<define>    add a preprocessor define\n");
	printf("    -o:<file>      output file\n");
	printf("    -dump          dump decompiled or preprocessed input file to output\n");
	printf("    -decompile     decompile binary resource to text\n");
	printf("    -?             display this help\n");
	printf("\n");
}



int main(int argc, char* argv[])
{
  	// Split command line
	CVector<CUniString> args;
	for (int i=1; i<argc; i++)
	{
		args.Add(a2w(argv[i]));
	}

	CUniString strError;
	if (!ExpandResponseFiles(args, strError))
	{
		wprintf(L"%s\n", strError);
		return 7;
	}

	CVector<CUniString> vecDefines;
	CVector<CUniString> vecIncludePaths;

	// Include path to self in search path
	CUniString strPathSelf;
	SplitPath(a2w(argv[0]), &strPathSelf, NULL);
	vecIncludePaths.Add(strPathSelf);


	CUniString strInputFile;
	CUniString strOutputFile;
	CResParser parser;

	bool bDecompile=false;
	bool bDump=false;

	for (int i=0; i<args.GetSize(); i++)
	{
		CUniString strSwitch, strValue;
		if (ParseArg(args[i], strSwitch, strValue))
		{
			if (IsEqualString(strSwitch, L"i"))
			{
				parser.AddIncludePath(strValue);
			}
			else if (IsEqualString(strSwitch, L"d"))
			{
				parser.Define(strValue, L"");
			}
			else if (IsEqualString(strSwitch, L"h") || IsEqualString(strSwitch, L"?"))
			{
				ShowHelp();
				return 1;
			}
			else if (IsEqualString(strSwitch, L"o"))
			{
				strOutputFile=strValue;
			}
			else if (IsEqualString(strSwitch, L"decompile"))
			{
				bDecompile=true;
			}
			else if (IsEqualString(strSwitch, L"dump"))
			{
				bDump=true;
			}
			else
			{
				wprintf(L"Unrecognised switch: %s\n", args[i]);
				return 7;
			}
		}
		else
		{
			strInputFile=args[i];
		}
	}

	// Check we got an input file
	if (IsEmptyString(strInputFile))
	{
		ShowHelp();
		printf("No input file specified\n");
		return 7;
	}

	if (bDecompile)
	{
		// Open file
		CFile file;
		result_t r;
		if ((r=file.OpenExisting(strInputFile))!=0)
		{
			printf("Failed to open '%S' - %S\n", strInputFile, FormatResult(r));
			return 7;
		}

		// Load in binary format
		CResNode node;
		if (r=LoadBinaryRes(&node, &file))
		{
			printf("Failed to load '%S' - %S\n", strInputFile, FormatResult(r));
			return 7;
		}

		if (bDump)
		{
			printf("%S\n", FormatResNode(&node, false));
		}
		else
		{
			// Work out output file name
			if (IsEmptyString(strOutputFile))
				strOutputFile=ChangeFileExtension(strInputFile, L"decompiled.src");

			// Save in text format
			if (r=SaveResNode(&node, strOutputFile))
			{
				printf("Failed to save '%S' - %s\n", strOutputFile, FormatResult(r));
				return 7;
			}
		}

		return 0;
	}
	else
	{
		// Parse it
		CResNode node;
		if (!parser.Parse(&node, strInputFile))
		{
			printf("%S\n\n", parser.GetError());
			return 7;
		}

		if (bDump)
		{
			printf("%S\n", FormatResNode(&node, false));
		}
		else
		{
			// Work out output file name
			if (IsEmptyString(strOutputFile))
				strOutputFile=ChangeFileExtension(strInputFile, L"srx");

			// Open file
			CFile file;
			result_t r;
			if ((r=file.CreateNew(strOutputFile))!=0)
			{
				printf("Failed to create '%S' - %S\n", strOutputFile, FormatResult(r));
				return 7;
			}

			// Save in binary
			if (r=SaveBinaryRes(&node, &file))
			{
				printf("Failed to create '%S' - %S\n", strOutputFile, FormatResult(r));
				return 7;
			}
		}

		// Done
		return 0;
	}



//	printf("%S\n\n", FormatResNode(&node, false));

	return 0;
}

