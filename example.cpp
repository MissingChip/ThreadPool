
#include "thread_pool.h"
#include <stdio.h>

void func(int a){
	printf("%d\n", a);	
}

int main(){
	ThreadPool pool(2);
	for(int i = 0; i < 20; i++){
		pool.submit_job(std::bind(func, i));
	}
}
