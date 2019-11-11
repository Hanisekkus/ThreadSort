#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <chrono>

using namespace std;

#define TYPE uint32_t

//template pro plneni ukolu
template <class T>
class TaskPart
{
public:
	uint32_t id;								//Id threadu
	int32_t from;								//Zacatek pole
	uint32_t lengthEnd;							//Konec pole
	vector<T> data;								//Pole s daty
	string order;								//Smer trideni
	std::chrono::duration<double> duration;		//cas behu

	TaskPart(uint32_t id, uint32_t from, uint32_t lengthEnd, vector<T> data, string order) :
		id(id), from(from), lengthEnd(lengthEnd), data(data), order(order) {}

	void Insertion()
	{
		for (uint32_t i = from + 1; i < from + lengthEnd; i++)
		{
			T temporary = data.at(i);
			int32_t j = i - 1;

			while (j >= from && ((order == "asc") ? data.at(j) > temporary:data.at(j) < temporary))
			{
				data.at(j + 1) = data.at(j);    //moves element forward
				j = j - 1;
			}

			data.at(j + 1) = temporary;				//insert element in proper place
		}
	}

	void Selection() {
		uint32_t minIndex;

		// One by one move boundary of unsorted subarray  
		for (uint32_t i = 0; i < lengthEnd - 1; i++)
		{
			// Find the minimum element in unsorted array  
			minIndex = i;
			for (uint32_t j = i + 1; j < lengthEnd; j++)
				if ((order == "asc") ? data.at(from + j) < data.at(from + minIndex) : data.at(from + j) > data.at(from + minIndex))
					minIndex = j;

			// Swap the found minimum element with the first element  
			T temporary = data.at(from + minIndex);
			data.at(from + minIndex) = data.at(from + i);
			data.at(from + i) = temporary;
		}
	}

	void BubbleSort() {
		for (uint32_t i = 0; i < lengthEnd - 1; i++)

			// Last i elements are already in place  
			for (uint32_t j = 0; j < lengthEnd - i - 1; j++)
				if ((order == "asc") ? data.at(from + j) > data.at(from + j + 1) :data.at(from + j) < data.at(from + j + 1))
				{
					T temporary = data.at(from + j);
					data.at(from + j) = data.at(from + j + 1);
					data.at(from + j + 1) = temporary;
				}
	}

	void PrintData() {
		cout << "Thread[" << id << "] : ";

		if (lengthEnd <= 100)
			for (uint32_t i = from; i < from + lengthEnd; i++) {
				cout << data[i] << " ";
			}
		else
			cout << "Too much element to print";

		cout << "\n    lasted: " << duration.count() * 1000 << "[ms]";
		cout << "\n    from: " << from << "[element]";
		cout << "\n    lenght: " << lengthEnd << "[elements]";
		cout << "\n\n";
	}
};

template <class U>
class RandomNumbers {
public:
	std::vector<U> randNumbers;

	void fullfill(U min, U max, uint32_t lengthNumbers) {
		U lowerBound = min;
		U upperBound = max;

		std::uniform_real_distribution<double> uniReal(lowerBound, upperBound);
		std::default_random_engine randomEngine;

		for (uint32_t i = 0; i < lengthNumbers; i++)
			randNumbers.push_back((U)uniReal(randomEngine));
	}

	void printVector() {
		for_each(randNumbers.begin(), randNumbers.end(), [](U &element) {
			cout << element << endl;
		});
	}
};

template<class T>
std::vector<T> MergeVectors(std::vector<T>mainVector, TaskPart<T>* task) {
	uint32_t i = 0;
	uint32_t j = task->from;

	std::vector<T> resultVector;

	while (i < mainVector.size() && j < task->from + task->lengthEnd) {
		if ((task->order == "asc") ? mainVector.at(i) < task->data.at(j) : mainVector.at(i) > task->data.at(j)) {
			resultVector.push_back(mainVector.at(i));
			i++;
		}
		else {
			resultVector.push_back(task->data.at(j));
			j++;
		}
	}
	while (i < mainVector.size()) {
		resultVector.push_back(mainVector.at(i));
		i++;
	}
	while (j < task->from + task->lengthEnd) {
		resultVector.push_back(task->data.at(j));
		j++;
	}
	return resultVector;
}

void printMergedVectors(std::vector<TYPE> MergeVectors)
{
	for (uint32_t i = 0; i<MergeVectors.size(); i++) {
		std::cout << MergeVectors.at(i);
		((i + 1) % 15 == 0) ? std::cout << std::endl : std::cout << " ";
	}
	std::cout << std::endl;
}

//**************
// Thread work
// *************

void threadWorkspace(uint32_t id, TaskPart<TYPE> *task)
{
	auto start = chrono::system_clock::now();

	cout << "Thread with id: " << id << "has started...\n";
	task->BubbleSort();

	auto end = chrono::system_clock::now();

	task->duration = end - start;
}

int main(int32_t t_na, char **t_arg)
{
	if (t_na != 4) {

		std::cout << "Wrong numbers of parametr!" << std::endl;
		return -1;
	}

	uint32_t length = atoi(t_arg[1]);
	uint32_t threads = atoi(t_arg[2]);
	std::string order = t_arg[3];

	std::vector<std::thread> workers;
	std::vector<TaskPart<TYPE>*> works;

	RandomNumbers<TYPE> randNumbers;
	randNumbers.fullfill((TYPE)0, (TYPE)100, length);

	uint32_t divided = length % threads;
	uint32_t temporaryToSpace = 0;
	for (uint32_t i = 0; i < threads; i++) {
		uint32_t temporaryEndSpace = (divided != 0) ? 1 : 0;

		works.push_back(new TaskPart<TYPE>(i, i * length / threads + temporaryToSpace, length / threads + temporaryEndSpace, randNumbers.randNumbers, order));
		workers.push_back(std::thread([i, works]() {threadWorkspace(i, works.at(i)); }));

		temporaryToSpace = (temporaryEndSpace != 0) ? 1 : 0;
		divided = (divided != 0) ? divided - 1 : divided;
	}

	for (uint32_t i = 0; i < workers.size(); i++) {
		workers[i].join();
	}

	double AllTime = 0;
	for (uint32_t i = 0; i < works.size(); i++) {
		works[i]->PrintData();
		AllTime = (works[i]->duration.count() > AllTime) ? works[i]->duration.count() : AllTime;
	}

	std::vector<TYPE>MergedVectors;
	for (uint32_t i = 0; i < works.size(); i++) {
		MergedVectors = MergeVectors(MergedVectors, works[i]);
	}

	printMergedVectors(MergedVectors);

	std::cout << "AllTime needed for " << threads << " threads: " << AllTime * 1000 << "[ms]" << std::endl;
	getchar();
	return 0;
}

//Run command .. g++ ./source.cpp -pthread -o out && ./out