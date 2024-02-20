#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers; 
  /* Additional fields here */
    u32 remaining_time;    // Time left for the process to finish
    u32 waiting_time;      // Total time the process has been waiting in the queue
    u32 response_time;     // Time from arriving to first getting scheduled
    u32 start_time;        // The time when the process starts its execution
    bool started;  
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  u32 cur_time = 0;
  bool any_process_arrived = false;

  // Initialize processes' states
  for (u32 i = 0; i < size; i++) {
      data[i].remaining_time = data[i].burst_time;
      data[i].started = false;
      if (data[i].arrival_time == 0) {
          TAILQ_INSERT_TAIL(&list, &data[i], pointers);
          any_process_arrived = true;
      }
  }

  // Main loop to simulate the round-robin scheduling
  while (size > 0) {
      // Check if any process is ready to be scheduled at the current time
      if (!any_process_arrived) {
          cur_time++;
          for (u32 i = 0; i < size; i++) {
              if (data[i].arrival_time == cur_time) {
                  TAILQ_INSERT_TAIL(&list, &data[i], pointers);
                  any_process_arrived = true;
              }
          }
      }

      // Schedule the process if the list is not empty
      if (any_process_arrived) {
          struct process *cur_proc = TAILQ_FIRST(&list);
          if (!cur_proc->started) {
              cur_proc->started = true;
              cur_proc->response_time = cur_time - cur_proc->arrival_time;
          }

          u32 time_slice = min(quantum_length, cur_proc->remaining_time);
          cur_time += time_slice;
          cur_proc->remaining_time -= time_slice;

          // Update arrival of new processes during the execution of the current process
          for (u32 time_passed = 0; time_passed < time_slice; time_passed++) {
              for (u32 i = 0; i < size; i++) {
                  if (data[i].arrival_time == cur_time - time_slice + time_passed + 1) {
                      TAILQ_INSERT_TAIL(&list, &data[i], pointers);
                  }
              }
          }

          // Process completion check
          if (cur_proc->remaining_time == 0) {
              cur_proc->waiting_time = cur_time - cur_proc->arrival_time - cur_proc->burst_time;
              total_waiting_time += cur_proc->waiting_time;
              total_response_time += cur_proc->response_time;
              size--;
              TAILQ_REMOVE(&list, cur_proc, pointers);
          } else {
              // Requeue the process for the next round
              TAILQ_REMOVE(&list, cur_proc, pointers);
              TAILQ_INSERT_TAIL(&list, cur_proc, pointers);
          }

          any_process_arrived = !TAILQ_EMPTY(&list);
      }
  }
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
