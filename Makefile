CXXFLAGS += -lpthread

example: example.cpp thread_pool.h

clean:
	rm example