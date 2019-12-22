#pragma once
#include "stdafx.h"
#include <fstream>

class Debug
{
public:
	static Debug* create()
	{
		static Debug debug;
		return &debug;
	}

	static void fill_with_zero(int len, float* outs)
	{
		for (int i = 0; i < len; i++)
		{
			outs[i] = 0;
		}
	}

	static int tr_date(float tdx_date)
	{
		return static_cast<int>(tdx_date) + 19000000;
	}

	static std::string tr_code(float* code)
	{
		char buf[7];
		sprintf(buf, "%06d", static_cast<int>(*code));
		buf[6] = '\0';
		return std::string(buf);
	}

	static void show(const char* msg)
	{
		MessageBoxA(NULL, msg, "错误", MB_OK);
	}

	void log(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		int size = vsprintf(buffer, format, args);
		va_end(args);
		if (size > 1023)
		{
			show("单次log超过缓冲区大小了");
			return;
		}
		ofs.write(buffer, size);
	}
private:
	Debug() {}
	std::ofstream ofs{ "duanxian.log" };
	char buffer[1024];
};
