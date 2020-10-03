CXXFLAGS += -lpthread -Wall --std=c++17

example: example.cpp thread_pool.h

clean:
	rm example example2
