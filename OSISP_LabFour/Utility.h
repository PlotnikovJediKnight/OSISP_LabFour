#pragma once

namespace Threading {
	namespace Utility {

		class TaskData
		{
		public:
			TaskData() = default;
			TaskData(const TaskData&) = delete;
			TaskData& operator=(const TaskData&) = delete;
			virtual ~TaskData() = 0 {};
		};

		template<typename ReturnType>
		struct ResultData {
			ReturnType result;
			ResultData(ReturnType result) : result(result) {}
		};

		template<typename ReturnType>
		using TaskFunc = ReturnType(*)(TaskData*);

		using SortRange = IteratorRange<std::vector<std::string>::iterator>;

		struct SortStructData : TaskData {
			SortRange range;
			SortStructData(SortRange range) : range(range) {}
		};
	}
}