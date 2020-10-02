
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

typedef int (job_fn_t)(void*);

//Convenience typedefs
typedef std::function<void()> job_t;
typedef std::unique_lock<std::mutex> ulock;

class ThreadPool {
public:
	ThreadPool(int thread_count);
	ThreadPool() : ThreadPool(std::max(2u, std::thread::hardware_concurrency())){};
	~ThreadPool() {if(!stopped) shutdown();};

	int thread_count;
	bool pool_terminate = false;
	bool stopped = false;
	std::vector<std::thread> pool;
	std::condition_variable condition;
	std::deque<job_t> job_q;
	std::mutex queue_mtx;
	std::mutex pool_mtx;

	void thread_fn();
	void submit_job(job_t job);
	void shutdown();
};

inline ThreadPool::ThreadPool(int thread_count){
	for(int i = 0; i < thread_count; i++){
		pool.push_back(std::thread(&ThreadPool::thread_fn, this));
	}
}

inline void ThreadPool::thread_fn(){
	while(true){
		job_t job;
		{
			ulock lock(queue_mtx);
			condition.wait(lock, [this]{return !job_q.empty() || pool_terminate;});
			if(pool_terminate && job_q.empty()) return;
			job = job_q.front();
			job_q.pop_front();
		}
		job();
	}
}

inline void ThreadPool::submit_job(job_t job){
	{
		ulock lock(queue_mtx);
		job_q.push_back(job);
	}
	condition.notify_one();
}

inline void ThreadPool::shutdown(){
	{
		ulock lock(pool_mtx);
		pool_terminate = true;
	}
	condition.notify_all();
	//
	for(auto& thread_a : pool){
		thread_a.join();
	}
	pool.clear();
	//
	stopped = true;
}


