#pragma once
#include "Task.h"
#include "Utility.h"
#include "Lock.h"
#include "DataStructures.h"
#include <stdexcept>
#include <vector>

using namespace Threading::DataStructures;

namespace Threading {

	template<typename ResultType>
	class Thread {
	private:

		HANDLE m_hthread;
		Task<ResultType>* m_ptask;
		bool alive;
		LPLONG lpCounter;
		ConcurrentQueue<Task<ResultType>*>* result_q;

	public:

		Thread() {
			m_hthread = nullptr;
			m_ptask = nullptr;
			result_q = nullptr;
			alive = true;
		}

		bool HasTask() {
			return m_ptask != nullptr;
		}

		~Thread() {
			End();
		}

		void SetBoolPointer(short* busy) {
			this->busy = busy;
		}

		void SetTask(Task<ResultType>* task) {
			m_ptask = task;
		}

		void SetResultQueue(ConcurrentQueue<Task<ResultType>*>* result_q) {
			this->result_q = result_q;
		}

		void SetCounterPointer(LPLONG lpTarget) {
			lpCounter = lpTarget;
		}

		void Begin() {
			m_hthread = (HANDLE)_beginthreadex(nullptr,
				0,
				&cThreadProc,
				(void*)this,
				0,
				nullptr);

			if (m_hthread == nullptr)
			{
				throw std::runtime_error("Thread has not been created!");
			}
		}

		void End() {
			alive = false;
			if (m_hthread != nullptr)
			{
				WaitForSingleObject(m_hthread, INFINITE);
				m_ptask = nullptr;
				CloseHandle(m_hthread);
				m_hthread = nullptr;
			}
		}

		DWORD ThreadProc() {
			while (alive)
			{

				if (m_ptask != nullptr)
				{
					{
						TaskFunc<ResultType> task = m_ptask->GetTask();
						TaskData* data = m_ptask->GetData();
						

						if (task != nullptr && data != nullptr)
						{
							task(data);
							m_ptask->r_data = new ResultData<ResultType>(
									 ((SortStructData*)data)->range
								);
							result_q->Push(m_ptask);
						}
						m_ptask = nullptr;
						InterlockedIncrement(lpCounter);
					}
				}
				Sleep(0);
			}

			return 0;
		}

	protected:
		static unsigned __stdcall cThreadProc(LPVOID _pThis)
		{
			return ((Thread*)_pThis)->ThreadProc();
		}
	};

}