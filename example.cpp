
#include "thread_pool.h"
#include <stdio.h>

// A class with a function to be called that takes no arguments
class MemberNoArg{
public:
	MemberNoArg(int a) : a{a} {};
	int a;
	void func() { printf("%d\n", a); };
	static void run_func(MemberNoArg* m) { m->func(); };
};

// A class with a function to be called that takes arguments
class MemberArg{
public:
	MemberArg(){};
	void func(int a) { printf("%d\n", a); };
};

// A function to be called that takes arguments
void func(int a){
	printf("%d\n", a);	
}

int main(){
	using namespace std;
	printf("Hardware Concurrency: %d\n", thread::hardware_concurrency());
	{
		ThreadPool pool(2);
		for(int i = 0; i < 5; i++){
			pool.submit_job([i]{func(i);}); // using a lambda function
			//pool.submit_job(std::bind(func, i)); // using std::bind()
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
			//pool.submit_job([&jobs, i]{func(i);}); // using a lambda function
			pool.submit_job(std::bind(&MemberNoArg::func, &jobs[i])); // using std::bind()
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
			//pool.submit_job([&jobs, i]{MemberNoArg::run_func(&jobs[i]);}); // using a lambda function
			pool.submit_job(std::bind(MemberNoArg::run_func, &jobs[i])); // using std::bind()
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
			pool.submit_job([&job, i]{job.func(i);}); // with a lambda
			// if this is possible with std::bind(), it's messy
		}
	}
}
