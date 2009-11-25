// SimpleResourceCompiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ResTemplate.h"

void ShowHelp()
{
	printf("Simple Resource Compiler v1.0\n");
	printf("Copyright (C) 2009 Topten Software. All Rights Reserved\n\n");
	printf("Usage:\tsrc [-?] [-i:<includepath>] [-d:<define>] [-o:<outputfile>] [-dump] [-decompile] [-unicode] [@<responsefile>] file \n\n");
	printf("    file           input file\n");
	printf("    -i:<path>      add include path\n");
	printf("    -d:<define>    add a preprocessor define\n");
	printf("    -t:<file>      template file\n");
	printf("    -to:<file>	   template output file\n");
	printf("    -o:<file>      binary output file\n");
	printf("    -p:<file>      processed output file\n");
	printf("    -dump          dump decompiled or preprocessed input file to output\n");
	printf("    -decompile     load src from binary file\n");
	printf("    -unicode       output text files in unicode\n");
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
		printf("%S\n", strError.sz());
		return 7;
	}

	CVector<CUniString> vecDefines;
	CVector<CUniString> vecIncludePaths;

	// Include path to self in search path
	CUniString strPathSelf;
	SplitPath(a2w(argv[0]), &strPathSelf, NULL);
	vecIncludePaths.Add(strPathSelf);


	CUniString strInputFile;
	CUniString strTextOutputFile;
	CUniString strBinaryOutputFile;
	CUniString strTemplateOutputFile;
	CUniString strTemplateFile;
	CResParser parser;

	bool bDecompile=false;
	bool bDump=false;
	bool bOutputInUnicode=false;

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
			else if (IsEqualString(strSwitch, L"t"))
			{
				strTemplateFile=strValue;
			}
			else if (IsEqualString(strSwitch, L"to"))
			{
				strTemplateOutputFile=strValue;
			}
			else if (IsEqualString(strSwitch, L"o"))
			{
				strBinaryOutputFile=strValue;
			}
			else if (IsEqualString(strSwitch, L"p"))
			{
				strTextOutputFile=strValue;
			}
			else if (IsEqualString(strSwitch, L"decompile"))
			{
				bDecompile=true;
			}
			else if (IsEqualString(strSwitch, L"dump"))
			{
				bDump=true;
			}
			else if (IsEqualString(strSwitch, L"unicode"))
			{
				bOutputInUnicode=true;
			}
			else
			{
				printf("Unrecognised switch: %S\n", args[i].sz());
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

	// Load input resource file
	CResNode node;
	if (bDecompile)
	{
		// Open file
		CFile file;
		result_t r;
		if ((r=file.OpenExisting(strInputFile))!=0)
		{
			printf("Failed to open '%S' - %S\n", strInputFile.sz(), FormatResult(r).sz());
			return 7;
		}

		// Load in binary format
		if ((r=LoadBinaryRes(&node, &file)))
		{
			printf("Failed to load '%S' - %S\n", strInputFile.sz(), FormatResult(r).sz());
			return 7;
		}
	}
	else
	{
		// Parse it
		if (!parser.Parse(&node, strInputFile))
		{
			printf("%S\n\n", parser.GetError());
			return 7;
		}
	}

	if (bDump)
	{
		printf("%S\n", FormatResNode(&node, false).sz());
	}

	if (!strTextOutputFile.IsEmpty())
	{
		// Save in text format
		CUniString strNode=FormatResNode(&node, false);
		result_t r;
		if (bOutputInUnicode)
		{
			r=SaveText<wchar_t>(strTextOutputFile, strNode);
		}
		else
		{
			r=SaveText<char>(strTextOutputFile, w2a(strNode));
		}
		if (r)
		{
			printf("Failed to save '%S' - %S\n", strTextOutputFile.sz(), FormatResult(r).sz());
			return 7;
		}
	}


	if (!strBinaryOutputFile.IsEmpty())
	{
		// Open file
		CFile file;
		result_t r;
		if ((r=file.CreateNew(strBinaryOutputFile))!=0)
		{
			printf("Failed to create '%S' - %S\n", strBinaryOutputFile.sz(), FormatResult(r).sz());
			return 7;
		}

		// Save in binary
		if ((r=SaveBinaryRes(&node, &file)))
		{
			printf("Failed to create '%S' - %S\n", strBinaryOutputFile.sz(), FormatResult(r).sz());
			return 7;
		}
	}

	// Load template
	if (!strTemplateFile.IsEmpty())
	{
		CResTemplate t;
		if (!t.ParseFile(strTemplateFile))
		{
			printf("Failed to parse template - %S\n", t.GetError());
			return 7;
		}

		CUniString strOutput;
		if (!t.Render(&node, strOutput))
		{
			printf("Failed to render template - %S\n", t.GetError());
			return 7;
		}

		printf("%S\n", strOutput);
	}

	// Done
	return 0;
}

