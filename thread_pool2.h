

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

//Convenience typedefs
typedef std::unique_lock<std::mutex> ulock;

template <class job_t = std::function<void()>>
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

	void thread_fn();
	void submit_job(job_t job);
	void shutdown();
};

template <class job_t>
inline ThreadPool<job_t>::ThreadPool(int thread_count){
	for(int i = 0; i < thread_count; i++){
		pool.push_back(std::thread(&ThreadPool::thread_fn, this));
	}
}

template <class job_t>
inline void ThreadPool<job_t>::thread_fn(){
	while(true){
		job_t job;
		{
			ulock lock(queue_mtx);
			condition.wait(lock, [this]{return !job_q.empty() || pool_terminate;});
			if(job_q.empty()){ // standard allows for spurrious wake-ups
				if(pool_terminate) return;
				continue;
			}
			job = job_q.front();
			job_q.pop_front();
		}
		job();
	}
}

template <class job_t>
inline void ThreadPool<job_t>::submit_job(job_t job){
	{
		ulock lock(queue_mtx);
		job_q.push_back(job);
	}
	condition.notify_one();
}

template <class job_t>
inline void ThreadPool<job_t>::shutdown(){
	{
		ulock lock(queue_mtx);
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
