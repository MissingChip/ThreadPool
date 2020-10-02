
#include "thread_pool.h"
#include <stdio.h>

class MemberNoArg{
public:
	MemberNoArg(int a) : a{a} {};
	int a;
	void func() { printf("%d\n", a); };
	static void run_func(MemberNoArg* m) { m->func(); };
};

class MemberArg{
public:
	MemberArg(){};
	void func(int a) { printf("%d\n", a); };
};

void func(int a){
	printf("%d\n", a);	
}

int main(){
	using namespace std;
	printf("Hardware Concurrency: %d\n", thread::hardware_concurrency());
	{
		ThreadPool pool(2);
		for(int i = 0; i < 5; i++){
			pool.submit_job(std::bind(func, i));
		}
	}
	printf("\n");
	{
		ThreadPool pool(2);
		std::vector<MemberNoArg> jobs;

		for(int i = 0; i < 5; i++){
			jobs.push_back(MemberNoArg(i));
		}
		for(int i = 0; i < 5; i++){
			pool.submit_job(std::bind(&MemberNoArg::func, &jobs[i]));
		}
		//If this doesn't happen, jobs could be destructed before thread pool
		//this could also be done using scope (destructor calls shutdown before jobs go out of scope)
		pool.shutdown();
	}
	printf("\n");
	{
		ThreadPool pool(2);
		std::vector<MemberNoArg> jobs;

		for(int i = 0; i < 5; i++){
			jobs.push_back(MemberNoArg(i));
		}
		for(int i = 0; i < 5; i++){
			pool.submit_job(std::bind(MemberNoArg::run_func, &jobs[i]));
		}
		//If this doesn't happen, jobs could be destructed before thread pool
		//this could also be done using scope (destructor calls shutdown before jobs go out of scope)
		pool.shutdown();
	}
	printf("\n");
	{
		ThreadPool pool;
		MemberArg job;
		for(int i = 0; i < 5; i++){
			pool.submit_job([&job, i]{job.func(i);});
		}
	}
}
