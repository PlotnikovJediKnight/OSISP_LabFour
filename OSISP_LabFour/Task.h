#pragma once
#include "Utility.h"
using namespace Threading::Utility;

namespace Threading {

	template<typename ReturnType>
	class Task {
	private:
		TaskFunc<ReturnType> m_task;
		TaskData* m_data;
		ResultData<ReturnType>* r_data;

		template<typename ReturnType>
		friend class Thread;

	public:
		Task(TaskFunc<ReturnType> task, TaskData* data) {
			m_task = task;
			m_data = data;
			r_data = nullptr;
		}

		Task(Task&& rhs) {
			m_task = rhs.m_task;
			m_data = rhs.m_data;
			r_data = rhs.r_data;

			rhs.m_task = nullptr;
			rhs.m_data = nullptr;
			rhs.r_data = nullptr;
		}

		~Task() {
			if (m_data != nullptr)
				delete m_data;
			if (r_data != nullptr)
				delete r_data;
			m_data = nullptr;
			r_data = nullptr;
			m_task = nullptr;
		}

		TaskFunc<ReturnType> GetTask() { return m_task; }
		TaskData* GetData() { return m_data; }
		ResultData<ReturnType>* GetResult() { return r_data; }
	};
}
