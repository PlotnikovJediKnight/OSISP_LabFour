#pragma once
#include <Windows.h>

namespace Threading {
	class MutexLock
	{
	private:
		CRITICAL_SECTION critical_section_;
	public:
		MutexLock() { ::InitializeCriticalSection(&critical_section_); }
		~MutexLock()	{ ::DeleteCriticalSection(&critical_section_); }
		void Lock() { ::EnterCriticalSection(&critical_section_); }
		void Unlock() { ::LeaveCriticalSection(&critical_section_);	}
	};
}