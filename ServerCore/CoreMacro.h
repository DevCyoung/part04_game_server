#pragma once

/*---------------------
	Crash
---------------------*/
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}											\

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH(__FILE__, __LINE__);			\
		__analysis_assume(expr);			\
	}										\
}											\
