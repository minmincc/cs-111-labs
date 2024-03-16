# Hey! I'm Filing Here

In this lab, I successfully implemented a 1 MiB ext2 file system with 2 directories, 1 regular file, and 1 symbolic link.

## Building

Run `make` to build the program

## Running

Compile the executable
`make`

Run the executable:
`./ext2-create`

Create a directory to mount:
`mkdir mnt`

Mount the filesystem:
`sudo mount -o loop cs111-base.img mnt`

To check the result: `cd mnt`, run:
`ls -ain`



## Cleaning up

Unmount and remove the mount directory:
`sudo umount mnt`

`rmdir mnt`

Clean up all binary files: 
`make clean`
