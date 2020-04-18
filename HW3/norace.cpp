// Using a pthread mutex to get rid of the race condition
// Must be compiled with the  -fpermissive and -lpthread options
// g++ -fpermissive norace.cpp -lpthread
// Can ignore the two warnings

#include <iostream>
#include <pthread.h>

using namespace std;

static int counter = 0;
static pthread_mutex_t lock;

void *child_thread(void *arg) {
	int i;
	int ntimes;
	ntimes = (int) arg;
	for( i = 0; i < ntimes ; i++) {
		pthread_mutex_lock(&lock);
			counter++;
		pthread_mutex_unlock(&lock);
	} // for
} // child_thread

int main() {
	pthread_t tid;
        pthread_mutex_init(&lock, NULL);
 
	int ntimes =100000;
	int i = 0;

	pthread_create(&tid, NULL, child_thread, (void *) ntimes);

	for( i = 0; i < ntimes ; i++) {
		pthread_mutex_lock(&lock);
			counter++;
		pthread_mutex_unlock(&lock);
	} //for
        
	pthread_join(tid, NULL);
        cout << "Final value of counter: " << counter << endl ;
} // main
