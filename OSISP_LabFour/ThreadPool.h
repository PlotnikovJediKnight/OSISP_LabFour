#pragma once
#include <string>
#include "DataStructures.h"
#include "Task.h"
#include "Thread.h"
#include "Utility.h"
#include "Paginator.h"

using namespace Threading::DataStructures;

template<typename ResultType>
class Thread;

namespace Threading {


	template <typename ResultType>
	class Threadpool {
	private:

		bool async_handler_enabled;
		HANDLE hAsyncHandlerThread = nullptr;

		ConcurrentQueue<Task<ResultType>*> m_qtaskList;
		ConcurrentQueue<Task<ResultType>*> m_resultList;

		vector<Thread<ResultType>*>	m_vthreads;

		friend class Thread<ResultType>;

		LONG free_threads_count;
		size_t expected_tasks;

		Threadpool() { async_handler_enabled = false; free_threads_count = 0; expected_tasks = 0; }
		Threadpool(const Threadpool&) = delete;
		Threadpool& operator=(const Threadpool&) = delete;
		~Threadpool()
		{
			Shutdown();
		}

		static unsigned __stdcall PassTaskToAvailableThread(LPVOID _pthis) {
			Threadpool<ResultType>* pool = (Threadpool<ResultType>*)(_pthis);
			while (pool->async_handler_enabled) {

				if (!pool->m_qtaskList.Empty() && 
					pool->free_threads_count > 0) {
					Task<ResultType>* task = pool->m_qtaskList.Pop();
					

					size_t i = 0;
					for (auto thread : pool->m_vthreads) {
						if (!thread->HasTask()) {
							InterlockedDecrement(&pool->free_threads_count);
							thread->SetTask(task);
							break;
						}
						i++;
					}
				}
			}

			return 0;
		}

	public:

		static Threadpool<ResultType>& Instance() {
			static Threadpool<ResultType> instance;
			return instance;
		}

		void Initialize(size_t fixed_threads_number) {
			m_vthreads.assign(fixed_threads_number, nullptr);
			free_threads_count = fixed_threads_number;

			for (size_t i = 0; i < fixed_threads_number; ++i) {
				m_vthreads[i] = new Thread<ResultType>();
				m_vthreads[i]->SetResultQueue(&m_resultList);
				m_vthreads[i]->SetCounterPointer(&free_threads_count);
				m_vthreads[i]->Begin();
			}
		}

		void Shutdown() {
			async_handler_enabled = false;
			Task<ResultType>* deltask;
			Thread<ResultType>* delthread;

			while (!m_vthreads.empty())
			{
				delthread = m_vthreads.back();
				m_vthreads.pop_back();
				delthread->End();
				delete delthread;
			}

			while (!m_qtaskList.Empty())
			{
				deltask = m_qtaskList.Pop();
				delete deltask;
			}
		}

		void AddTask(Task<ResultType>* newTask) {
			expected_tasks++;
			m_qtaskList.Push(newTask);
		}

		void BeginProcessing() {
			async_handler_enabled = true;
			hAsyncHandlerThread = (HANDLE)_beginthreadex(nullptr,
				0,
				PassTaskToAvailableThread,
				(void*)this,
				0,
				nullptr);
		}

		void WaitProcessing() {
			cout << "Waiting for threads to finish..." << endl;
			while (m_resultList.Size() != expected_tasks) { Sleep(0); }
			cout << "The threads have finished working!" << endl;
			expected_tasks = 0;
		}

		std::vector<Task<ResultType>*> GetCompletedTasks() {
			std::vector<Task<ResultType>*> results;
			while (!m_resultList.Empty()) {
				results.push_back(m_resultList.Pop());
			}
			return results;
		}
	};

}