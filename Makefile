CXXFLAGS += -lpthread -Wall --std=c++17

example: example.cpp thread_pool.h

example2: example2.cpp thread_pool2.h

clean:
	rm example example2
