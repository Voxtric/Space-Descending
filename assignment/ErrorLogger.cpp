#include "errorlogger.h"
#include <math.h>

const wchar_t ErrorLogger::Filename[]=L"error.log";
ErrorLogger ErrorLogger::instance;
int ErrorLogger::LineCount=0;


ErrorLogger::ErrorLogger()
{
#ifdef LOGGING
	file.open(Filename);
#endif
}

ErrorLogger::~ErrorLogger()
{
#ifdef LOGGING
	file.close();
#endif
}

void ErrorLogger::Writeln(const wchar_t text[])
{
	Write(text);
	Write(L"\n");
}

void ErrorLogger::Write(const wchar_t text[])
{
#ifdef LOGGING
	if(LineCount<MAXLINES)
	{
		OutputDebugString(text);
		instance.file << text;
		if(++LineCount == MAXLINES)
		{
			OutputDebugString(L"\nErrorLogger limit reached. Who taught you to progam?");
			instance.file << L"\nErrorLogger limit reached. Who taught you to progam?";
			instance.file.flush();
		}
	}
#endif
#ifdef SLOWLOG
	instance.file.flush();
#endif
}

void ErrorLogger::Writeln(double num)
{
	Write(num);
	Write(L"\n");
}

void ErrorLogger::Write(double num)
{
#ifdef LOGGING
	wchar_t buffer[32];
	swprintf_s( buffer,32, L"%.8g", num );
	Write(buffer);
#endif
}

