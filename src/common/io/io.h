#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stdio.h>

/*
  ┌───────────────────────────────┐
  │   FILESIZE                    │
  └───────────────────────────────┘
 */

// get the size of a file
// @param fd: file descriptor
// @param filename: the file to get the size of
// @return the size of the file
long file_size(FILE *fd, char *filename);

/*
  ┌───────────────────────────────┐
  │   FMMAP                       │
  └───────────────────────────────┘
 */

// Read a file using mmap (PROT_READ, MAP_PRIVATE)
// @param fd: file descriptor
// @param filename: the file to mmap
// @param FileSize: the size of the file to mmap
// @return the mmap'd file
char *fmmap(FILE *fd, long long FileSize);

/*
  ┌───────────────────────────────┐
  │   SAFE_MUNMAP                 │
  └───────────────────────────────┘
 */

// munmap with proper error handling
// @param file: the file to munmap
// @param filesize: the size of the file to munmap
// @return true if successful, false otherwise
bool safe_munmap(char *file, long filesize);

/*
  ┌───────────────────────────────┐
  │   CREATE FILE                 │
  └───────────────────────────────┘
 */

// create a file
// @param filename: the file to create
// @param data: the data to write to the file
// @return true if successful, false otherwise
bool create_file(char *filename, char *data);

/*
  ┌───────────────────────────────┐
  │   READ FILE | FMMAP           │
  └───────────────────────────────┘
 */

// read a file using fmmap
// @param fd: file descriptor
// @param filename: the file to read
// @param filesize: return the size of the file
// @return file contents
char *read_file_fmmap(FILE *fd, char *filename, long *filesize);

/*
  ┌───────────────────────────────┐
  │   WRITE FILE | FMMAP          │
  └───────────────────────────────┘
 */

// write to a file using fmmap
// @param fd: file descriptor
// @param filename: the file to write to
// @param data: the data to write to the file
// @return true if successful, false otherwise
bool write_file_fmmap(FILE *fd, char *filename, char *data);

/*
  ┌───────────────────────────────┐
  │   CREATE FOLDER               │
  └───────────────────────────────┘
 */

// create a folder using mkdir
// @param folder: the folder to create
// @return true if successful, false otherwise
bool create_folder(char *foldername);
/*
  ┌───────────────────────────────┐
  │   FILE EXISTS?                │
  └───────────────────────────────┘
 */

// check if a file exists with ambiguous error handling
// @param fd: file descriptor
// @param filename: the file to check
// @return true if the file exists, false otherwise
bool file_exists(FILE *fd, char *filename);
/*
  ┌───────────────────────────────┐
  │   FOLDER EXISTS?              │
  └───────────────────────────────┘
 */

// check if a folder exists with ambiguous error handling
// @param fd: file descriptor
// @param foldername: the folder to check
// @return true if the folder exists, false otherwise
bool folder_exists(FILE *fd, char *foldername);

/*
  ┌───────────────────────────────┐
  │   PRINTING                    │
  └───────────────────────────────┘
 */

// print a file
// @param fd: file descriptor
// @param filename: the file to print
void print_file(FILE *fd, char *filename);

#endif