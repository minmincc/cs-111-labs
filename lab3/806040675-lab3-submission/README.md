# Hash Hash Hash
TODO introduction

## Building
```shell
TODO
```
Command:
“`make`"

## Running
```shell
TODO how to run and results
```
Answer:
`./hash-table-tester -t [number of threads] -s [number of entries]`

example command and its output:
./hash-table-tester -t 8 -s 50000

Generation: 71,070 usec
Hash table base: 1,214,825 usec
  - 0 missing
Hash table v1: 1,532,331 usec
  - 0 missing
Hash table v2: 392,594 usec
  - 0 missing


## First Implementation
In the `hash_table_v1_add_entry` function, I added TODO

I've  initializing by "`pthread_mutex_t mutex`" to synchronize access to the entire hash table, ensuring that only one thread can modify the table at a time. When a thread adds or updates an entry in function "`hash_table_v2_add_entry`", it locks this mutex, blocking other threads from accessing the table until the operation is complete and the mutex is unlocked. This design guarantees thread safety by serializing all write operations but at the cost of potential performance bottlenecks due to lock contention, especially with many concurrent threads. The mutex is properly destroyed in the cleanup function "`hash_table_v2_destroy`" to avoid resource leaks.

### Performance
```shell
TODO how to run and results
example 1: ./hash-table-tester -t 8 -s 50000
Generation: 71,070 usec
Hash table base: 1,214,825 usec
  - 0 missing
Hash table v1: 1,532,331 usec
  - 0 missing
Hash table v2: 392,594 usec
  - 0 missing
example 2:  ./hash-table-tester -t 4 -s 50000
Generation: 35,259 usec
Hash table base: 219,806 usec
  - 0 missing
Hash table v1: 390,257 usec
  - 0 missing
Hash table v2: 71,679 usec
  - 0 missing
```
Version 1 is a little slower/faster than the base version. As TODO

Answer:
For both scenarios, whether operating with fewer or more threads, the performance of v1 lags behind that of the base version. This slowdown arises from the necessity for threads to pause operations while awaiting the completion of entry additions by the active thread. Additionally, the process of acquiring and releasing mutex locks contributes to this delay.

This observation further clarifies why v1's execution time improves with fewer threads compared to a higher thread count. With a reduced number of threads, the overall wait time for mutex access diminishes, leading to more efficient lock management and shorter wait periods for thread operations.

## Second Implementation
In the `hash_table_v2_add_entry` function, I TODO

Answer:
In the v2 implementation, I've employed a distinct `pthread_mutex_t` mutex for every hash table entry, initializing these mutexes during the hash table's creation. This setup allows for individual entry locking in the `hash_table_v2_add_entry` function. Each entry is securely locked during updates or additions, ensuring that while a thread modifies one entry, other threads can concurrently interact with different entries without interference. Upon completion, all entry-specific mutexes are systematically destroyed in the `hash_table_v2_destroy` function.

This method is valid as it secures the essential section of each entry. By locking an entry during updates or additions, it prevents concurrent access to the same entry by other threads, allowing them to operate on different entries simultaneously. This approach not only prevents data inconsistencies and errors but also boosts overall throughput by enabling more efficient multi-threaded operations.

### Performance
```shell
TODO how to run and results
```
TODO more results, speedup measurement, and analysis on v2

Answer:
example:  ./hash-table-tester -t 4 -s 50000
Generation: 35,259 usec
Hash table base: 219,806 usec
- 0 missing
Hash table v1: 390,257 usec
- 0 missing
Hash table v2: 71,679 usec
- 0 missing

Using the command ./hash-table-tester -t 4 -s 50000, the v2 implementation demonstrated a significant performance improvement over the base version, completing in 71,679 microseconds compared to the base's 219,806 microseconds. This results in a speedup of approximately 3 times faster with the machine's 4 physical cores fully utilized.

Notice the difference between v2 and v1 lies in the locking strategy. While v1 employs a lock that serializes access to the hash table, leading to increased waiting times for threads, v2 utilizes individual locks for each hash table entry. This fine-grained locking approach minimizes contention and allows multiple threads to modify different parts of the hash table concurrently, drastically reducing wait times and enhancing performance, especially in a multi-core environment.


## Cleaning up
```shell
TODO how to clean
```
Command:
"`make clean`"