#pragma once
#include "stdafx.h"

#ifndef LOGOFF
#include <fstream>
#endif // !LOGOFF

class Debug
{
public:
	static inline Debug* create()
	{
		static Debug debug;
		return &debug;
	}

	//static void fill_with_zero(int len, float* outs)
	//{
	//	for (int i = 0; i < len; i++)
	//	{
	//		outs[i] = 0;
	//	}
	//}

	//static int tr_date(float tdx_date)
	//{
	//	return static_cast<int>(tdx_date) + 19000000;
	//}

	//static std::string tr_code(float* code)
	//{
	//	char buf[7];
	//	sprintf(buf, "%06d", static_cast<int>(*code));
	//	buf[6] = '\0';
	//	return std::string(buf);
	//}

	static inline void show(const char* msg)
	{
		MessageBoxA(NULL, msg, "错误", MB_OK);
	}

#ifndef LOGOFF
	static inline void show2(const char* format, ...)
	{
		char buffer[128];
		va_list args;
		va_start(args, format);
		int size = vsprintf(buffer, format, args);
		va_end(args);
		show(buffer);
	}
#endif // !LOGOFF

	void inline log(const char* format, ...)
	{
#ifndef LOGOFF
		char buffer[128];
		va_list args;
		va_start(args, format);
		int size = vsprintf(buffer, format, args);
		va_end(args);
		if (size > 127)
		{
			show("单次log超过缓冲区大小了");
			return;
		}
		ofs.write(buffer, size);
#endif
	}
private:
	Debug() {}
#ifndef LOGOFF
	std::ofstream ofs{ "duanxian.log" };
#endif // !LOGOFF
};
