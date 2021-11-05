#include <iostream>
#include <thread>
#include <Windows.h>
#include <algorithm>
#include "FileReader.h"
#include "IteratorRange.h"
#include "ThreadPool.h"
#include "profile.h"
using namespace std;
using namespace Threading;

const size_t MAX_THREADS = thread::hardware_concurrency();
size_t user_threads = 0;

char path[256] = "\0";

char resPath[256] = "D:\\result.txt";

vector<string> words;
vector<Task<SortRange>*> tasks;
vector<Task<SortRange>*> completedTasks;


SortRange SortThreadProc(TaskData* data) {
	SortStructData* downcast = dynamic_cast<SortStructData*>(data);
	sort(downcast->range.begin(), downcast->range.end());
	return downcast->range;
}

auto GetPaginatedTasks() {
	size_t words_total = words.size();
	size_t page_size = words_total / user_threads;
	if (page_size == 0) page_size = 1;
	vector<Task<SortRange>*> tasks;

	for (auto it : Paginate(words, page_size)) {
		tasks.push_back(new Task<SortRange>{ &SortThreadProc, new SortStructData{ it } });
	}

	return tasks;
}

void MergeAltogether(vector<Task<SortRange>*>&);

void SortRoutine() {
	Threadpool<SortRange>::Instance().Initialize(user_threads);
	Threadpool<SortRange>::Instance().BeginProcessing();
	{
		LOG_DURATION("SortRoutine");
		words = FileReader::Instance().GetFileStrings();
		tasks = GetPaginatedTasks();
		for (auto& task : tasks) {
			Threadpool<SortRange>::Instance().AddTask(task);
		}
		Threadpool<SortRange>::Instance().WaitProcessing();
		completedTasks = Threadpool<SortRange>::Instance().GetCompletedTasks();
	}
	Threadpool<SortRange>::Instance().Shutdown();
	MergeAltogether(completedTasks);
}

void MergeAltogether(vector<Task<SortRange>*>& completedTasks) {
	sort(completedTasks.begin(), completedTasks.end(), [](Task<SortRange>* lhs, Task<SortRange>* rhs){
		return lhs->GetResult()->result.begin() <
			   rhs->GetResult()->result.begin();
	});

	auto vec_begin = completedTasks[0]->GetResult()->result.begin();

	for (size_t i = 1; i < completedTasks.size(); i++) {
		auto taskRes = completedTasks[i]->GetResult()->result;
		inplace_merge(vec_begin, taskRes.begin(), taskRes.end());
	}
}

void PrintChosenFilePath() {
	const char* file = FileReader::Instance().GetFilePath();
	if (file == nullptr || file[0] == '\0') {
		cerr << "File was not chosen yet!" << endl;
	} else {
		cout << file << endl;
	}
}

void ChooseAndReadFile() {
	cout << "Please, enter file path:" << endl;
	cin.ignore(1);
	cin.getline(path, 30);
	FileReader::Instance().SetFilePath(path);
	FileReader::Instance().DoReadFile();
}

void PrintMenu() {
	cout << "=====================Main Menu=====================" << endl;
	cout << "0. Print Menu" << endl;
	cout << "1. Choose & Read File" << endl;
	cout << "2. Print Chosen File Path" << endl;
	cout << "3. Sort Chosen File & Print Result" << endl;
	cout << "4. Sort Chosen File & Save Result as File" << endl;
	cout << "5. Exit" << endl;
}

int main() {
	setlocale(LC_ALL, "RUS");

	cout << "Please, enter the number of threads you would like to use (max " << MAX_THREADS << " is allowed):" << endl;
	cin >> user_threads;

	if (user_threads > MAX_THREADS || user_threads == 0) {
		cout << "Sorry, cannot do that. Bye!" << endl;
		return -1;
	}

	PrintMenu();

	while (true) {
		cout << "Enter your command, please:" << endl;
		size_t command = 0;
		cin >> command;

		switch (command) {

		case 0: {
			PrintMenu();
			break;
		}

		case 1: {
			ChooseAndReadFile();
			break;
		}

		case 2: {
			PrintChosenFilePath();
			break;
		}

		case 3: {
			SortRoutine();
			cout << "Result:" << endl;
			for (string& s : words) {
				cout << s << endl;
			}
			break;
		}

		case 4: {
			SortRoutine();
			ofstream out(resPath);
			out << "Result:" << endl;
			for (string& s : words) {
				out << s << endl;
			}
			out.close();
			break;
		}

		case 5: {
			return 0;
		}

		}
	}

	return 0;
}