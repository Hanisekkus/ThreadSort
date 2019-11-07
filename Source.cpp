#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <chrono>

using namespace std;

#define ORDER "ASC"
#define TYPE double
#define THREADS 6
#define LENGTH 100000

//template pro plneni ukolu
template <class T>
class TaskPart 
{
public:
	uint32_t id;								//Id threadu
	uint32_t from;								//Zacatek pole
	uint32_t lengthEnd;							//Konec pole
	vector<T> data;								//Pole s daty
	string order;								//Smer trideni
	std::chrono::duration<double> duration;		//cas behu

	TaskPart(uint32_t id, uint32_t from, uint32_t lengthEnd, vector<T> data, string order) :
		id(id), from(from), lengthEnd(lengthEnd), data(data), order(order) {}

	void Insertion()
	{
		for (uint32_t i = 1; i <= lengthEnd - 1; i++)
		{
			T temporary = data.at(from + i);
			int32_t j = i - 1;

			while ((temporary < data.at(from + j)) && (j >= 0))
			{
				data.at(from + j + 1) = data.at(from + j);    //moves element forward
				j = j - 1;
			}

			data.at(from + j + 1) = temporary;				//insert element in proper place
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
				if (data.at(from + j) < data.at(from + minIndex))
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
				if (data.at(from + j) > data.at(from + j + 1))
				{
					T temporary = data.at(from + j);
					data.at(from + j) = data.at(from + j + 1);
					data.at(from + j + 1) = temporary;
				}

	}

	void PrintData() {
		cout << "Thread[" << id << "] : ";

		if (lengthEnd <= 100)
			for_each(data.begin() + from, data.begin() + from + lengthEnd, [](T &Temporary)
			{
				cout << Temporary << " "; 
			});
		else
			cout << "Too much element to print";

		cout << "\n    lasted: " << duration.count()*1000 << "[ms]";
		cout << "\n    lenght: " << lengthEnd << "[elements]";
		cout << "\n\n";
	}

};

template <class U>
class RandomNumbers {
public:
	std::vector<U> randNumbers;

	void fullfill(U min, U max,uint32_t lengthNumbers) {
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

void threadWorkspace(uint32_t id, TaskPart<TYPE>* task)
{
	auto start = chrono::system_clock::now();

	cout << "Thread with id: " << id << "has started...\n";
	task->Selection();

	auto end = chrono::system_clock::now();

	task->duration = end - start;
}

int main(int t_na, char **t_arg) 
{
	std::vector<std::thread> workers;
	std::vector<TaskPart<TYPE>*> works;

	RandomNumbers<TYPE> randNumbers;
	randNumbers.fullfill((TYPE)0, (TYPE)100, LENGTH);

	uint32_t divided = LENGTH % THREADS;
	for (uint32_t i = 0; i < THREADS; i++) {
		uint32_t temporaryEndSpace = (divided != 0) ? 1 : 0;
		uint32_t temporaryToSpace = 0;

		works.push_back(new TaskPart<TYPE>(i, i * LENGTH / THREADS + temporaryToSpace, LENGTH / THREADS + temporaryEndSpace, randNumbers.randNumbers, ORDER));
		workers.push_back(std::thread([i, works]() {threadWorkspace(i, works.at(i)); }));

		divided = (divided != 0)? divided - 1 : divided;
		temporaryToSpace = (divided != 0) ? temporaryToSpace++ : temporaryToSpace;
	}

	std::for_each(workers.begin(), workers.end(), [](std::thread &barrier) {
		barrier.join();
	});

	static double AllTime;
	std::for_each(works.begin(), works.end(), [](TaskPart<TYPE> *barrier) {
		barrier->PrintData();
		AllTime = (barrier->duration.count() > AllTime) ? barrier->duration.count() : AllTime;
	});

	cout << "AllTime needed: " << AllTime*1000 << "[ms]";

	getchar();
	return 0;
}