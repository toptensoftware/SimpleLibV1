#include "../SimpleLibUtils.h"

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

class CTest
{
public:
	CTest(const char* pszName)
	{
		printf("Testing %s...", pszName);
		g_bFailed=false;
	}
	~CTest()
	{
		if (!g_bFailed)
			printf("OK\n");
	}
};

int main()
{
	{
		CTest t("CRC");
		ASSERT(CalculateCRC("Hello World", 11)==0x4a17b156);
	}

	{
		CTest t("Formatted Binary");
		CUniString str=FormatBinaryData("Hello World", 11, 0);
		ASSERT(IsEqualString(str, L"48656C6C6F20576F726C64"));
		void* p;
		size_t cb;
		ASSERT(UnformatBinaryData(str, &p, &cb));
		ASSERT(cb==11);
		ASSERT(IsEqualString(CAnsiString((char*)p, cb), "Hello World"));
	}

	{
		CTest t("Increment String");
		ASSERT(IsEqualString(IncrementString(L"Item23"), L"Item24"));
		ASSERT(IsEqualString(IncrementString(L"Item"), L"Item1"));
		ASSERT(IsEqualString(IncrementStringEx(L"Item", L" (001)"), L"Item (001)"));
	}

	{
		CTest t("Load/save Unicode Text File");
		CUniString strContent=L"This is a text file\n";
		ASSERT(SaveTextFile(L"TestU.txt", strContent));
		CUniString strLoaded;
		ASSERT(LoadTextFile(L"TestU.txt", strLoaded));

		ASSERT(IsEqualString(strContent, strLoaded));

	}
	{
		CTest t("Load/save Ansi Text File");
		CAnsiString strContent="This is a text file\n";
		ASSERT(SaveTextFile("TestA.txt", strContent));
		CAnsiString strLoaded;
		ASSERT(LoadTextFile("TestA.txt", strLoaded));

		ASSERT(IsEqualString(strContent, strLoaded));
	}

	{
		CTest t("String Replace");
		ASSERT(IsEqualString(StringReplace(L"$(X)texttexttext", L"$(X)", L"ZZZ", false), L"ZZZtexttexttext"));
		ASSERT(IsEqualString(StringReplace(L"$(X)texttexttext$(X)", L"$(X)", L"ZZZ", false, 1), L"$(X)texttexttextZZZ"));
		ASSERT(IsEqualString(StringReplace(L"$(X)texttexttext$(X)", L"$(x)", L"ZZZ", true), L"$(X)texttexttext$(X)"));
		ASSERT(IsEqualString(StringReplace(L"$(X)texttexttext$(X)", L"$(x)", L"ZZZ", false), L"ZZZtexttexttextZZZ"));
	}

	{
		CTest t("Path Library");
		printf("\n");

		ASSERT(IsEqualString(SimplePathAppend(L"parta", L"partb"), L"parta/partb"))
		ASSERT(IsEqualString(SimplePathAppend(L"parta/", L"partb"), L"parta/partb"))
		ASSERT(IsEqualString(SimplePathAppend(L"parta", L"/partb"), L"parta/partb"))
		ASSERT(IsEqualString(SimplePathAppend(L"parta/", L"/partb"), L"parta/partb"))

		ASSERT(IsEqualString(FindRelativePath(L"/home/bradr/dev2/", L"/home/bradr/dev/file.txt"), L"../dev/file.txt"))
		ASSERT(IsEqualString(FindRelativePath(L"/home/Bradr/dev2/", L"/home/bradr/dev/file.txt"), L"../../bradr/dev/file.txt"))

	CUniString stCurrent=CurrentDirectory();

		printf("\tCurrent directory: %S\n", CurrentDirectory().sz());

		#define show(x) printf("\t%S --> %S\n", x, QualifyPath(x).sz());
		show(L".");
		show(L"../../blah.txt")
		#undef show

		CUniString L, R;
		#define show(x) L.Empty(); R.Empty(); SplitPath(x, &L, &R); printf("\t%S --> %S  and  %S\n", x, L.sz(), R.sz());
		show(L"/var/temp/file.txt/");
		show(L"/var/temp/file.txt");
		#undef show
	}

    return 0;
}
