#include "arena.h"
#include <iostream>
#include "SkipList.h"
#include "pthread.h"  

struct Comparator {
	int operator()(const int& a, const int& b) const {
		if (a < b) {
			return -1;
		}
		else if (a > b) {
			return +1;
		}
		else {
			return 0;
		}
	}
};


Arena arena;
Comparator cmp;
SkipList<int, Comparator> list(cmp, &arena);


void *Function_t(void *rank) {
	intptr_t myrank = reinterpret_cast<intptr_t>(rank);

	if (myrank==0) {
		list.Insert(10);
		list.Insert(24);
		list.Insert(92);
		list.Insert(31);
		list.Insert(6);
		list.Insert(4);
		list.Insert(14);
		list.Insert(7);
		list.Insert(9);

		SkipList<int, Comparator>::Iterator it(&list);
		it.SeekToFirst();
		while (it.Valid()) {
			std::cout << it.key() << std::endl;
			it.Next();

		}
	}

	if (myrank == 1) {
		list.Insert(41);
		list.Insert(11);
		list.Insert(12);
		list.Insert(15);
		list.Insert(615);
	
	}

	if (myrank == 2) {
		
		list.Insert(545);
		list.Insert(54);
		list.Insert(55);
		list.Insert(5);
		list.Insert(51);

		list.Insert(52);
	}




	return nullptr;
}



int main() {





	intptr_t thread;
	pthread_t* thread_handles;


	int thread_count = 5;

	thread_handles = new pthread_t[thread_count];

	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, Function_t, (void*)thread);

	std::cout << "Hello from main thread!" << std::endl;

	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);

	delete[] thread_handles;


	return 0;
	
	
}