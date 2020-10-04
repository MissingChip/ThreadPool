# ThreadPool
C++ thread pool

A thread pool class which can run with any function-like object. 
Uses the maximum amount of concurrent threads for a given machine by default,
as defined by std::thread::hardware_concurrency()

Example usage:

```C++
ThreadPool pool;

pool.submit_job(function_to_run)
```

More detailed examples in `example.cpp`
