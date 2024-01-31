# A Kernel Seedling
TODO: intro
Modify the proc_count.c file and then create a /proc/count
and finally get the current running process number through 'cat /proc/count'

## Building
```shell
TODO: cmd for build
```
command "make" to build module 

## Running
```shell
TODO: cmd for running binary 

```
TODO: results?

cmd for inserting the module into the kernel:
sudo insmod proc_count.ko

cmd for sanity check your module information:
proc_count.ko

cmd for Read any information messages printed in the kernel:
sudo dmesg -l info

expect result: use command 'cat /proc/count' should output a single integer number representing the number of process.

## Cleaning Up
```shell
TODO: cmd for cleaning the built binary
```
cmd for Remove the module from the kerne:
sudo rmmod proc_count


## Testing
```python
python -m unittest
```
TODO: results?
Ran 3 tests in 9.136s
ok

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
TODO: kernel ver?
5.14.8-arch1-1