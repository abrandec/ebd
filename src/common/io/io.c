// Ty Scott Kuhl
// https://github.com/skuhl/sys-prog-examples/blob/master/simple-examples/mmap.c

#include "io.h"

#include "../../errors/errors.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  ┌───────────────────────────────┐
  │   FILESIZE                    │
  └───────────────────────────────┘
 */

long file_size(FILE *fd, char *filename) {
  if (!filename) {
    custom_error(IO_FILENAME_NULL);
    return 0;
  }

  fd = fopen(filename, "r");

  if (!fd) {
    custom_error(IO_FILE_OPEN_FAIL);
    return 0;
  }

  fseek(fd, 0, SEEK_END);
  // get file size
  long ret = ftell(fd);

  fclose(fd);

  return ret;
}

/*
  ┌───────────────────────────────┐
  │   FMMAP                       │
  └───────────────────────────────┘
 */

char *fmmap(FILE *fd, long long FileSize) {
  fd = (FILE *)fileno(fd);
  // fd NULL checks need to be done outside of this function!
  return mmap(0, FileSize, PROT_READ, MAP_PRIVATE, (int)fd, 0);
}

/*
  ┌───────────────────────────────┐
  │   SAFE_MUNMAP                 │
  └───────────────────────────────┘
 */

bool safe_munmap(char *file, long filesize) {
  if (!file) {
    return false;
  }

  munmap(file, filesize);
  // not sure if this is necessary
  /* if (munmap(file, filesize) == -1) {
    perror("munmap");
    return false;
  } */

  return true;
}

/*
  ┌───────────────────────────────┐
  │   CREATE FILE                 │
  └───────────────────────────────┘
 */

bool create_file(char *filename, char *data) {
  if (!filename) {
    custom_error(IO_FILENAME_NULL);
    return false;
  }

  int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

  size_t textsize = strlen(data) + 1; // + \0 null character

  if (write(fd, "", textsize - 1) == -1) {
    close(fd);
    perror("Error writing last byte of the file");
    exit(EXIT_FAILURE);
  }

  char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    close(fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < textsize; ++i) {
    map[i] = data[i];
  }

  printf("%s\n", map);
  
  // Write it now to disk
  
 if (msync(map, textsize, MS_SYNC) == -1) {
    perror("Could not sync the file to disk");
  }
  
  // Don't forget to free the mmapped memory
  if (munmap(map, textsize) == -1) {
    close(fd);
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(fd);

  return true;
}

/*
  ┌───────────────────────────────┐
  │   READ FILE | FMMAP           │
  └───────────────────────────────┘
 */

char *read_file_fmmap(FILE *fd, char *filename, long *filesize) {
  if (!filename) {
    custom_error(IO_FILENAME_NULL);
    return NULL;
  }

  char *str;
  fd = fopen(filename, "r");
  // check if file exists
  if (!fd) {
    custom_error(IO_FILE_OPEN_FAIL);
    return NULL;
  }
  // get file position
  fseek(fd, 0, SEEK_END);

  // get file size
  long filesize_ = ftell(fd);

  // read file
  str = fmmap(fd, ftell(fd));

  // fclose does not unmap file so it's fine to call & it's portable
  // https://pubs.opengroup.org/onlinepubs/7908799/xsh/mmap.html
  fclose(fd);

  *filesize = filesize_;

  return str;
}

/*
  ┌───────────────────────────────┐
  │   WRITE FILE | FMMAP          │
  └───────────────────────────────┘
 */

bool write_file_fmmap(FILE *fd, char *filename, char *data) {
  if (!filename) {
    custom_error(IO_FILENAME_NULL);
    return false;
  }

  if (!data) {
    custom_error(IO_DATA_NULL);
    return false;
  }

  fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

  // check if file exists
  if (!fd) {
    custom_error(IO_FILE_OPEN_FAIL);
    return false;
  }

  // not finished lol
  return true;
}

/*
  ┌───────────────────────────────┐
  │   CREATE FOLDER               │
  └───────────────────────────────┘
 */

bool create_folder(char *foldername) {
  errno = 0;
  int ret = mkdir(foldername, S_IRWXU);

  if (ret == -1) {
    switch (errno) {
    case EACCES:
      custom_error(IO_WRITE_ACCESS_ERR);
      return false;
    case EEXIST:
      custom_error(IO_FOLDER_NAME_ALREADY_EXISTS);
      return false;
    case ENAMETOOLONG:
      custom_error(IO_FOLDER_NAME_TOO_LONG);
      return false;
    default:
      perror("mkdir");
      return false;
    }
  }

  return true;
}

/*
  ┌───────────────────────────────┐
  │   FILE EXITS?                 │
  └───────────────────────────────┘
 */

bool file_exists(FILE *fd, char *filename) {
  if (!filename) {
    return false;
  }

  fd = fopen(filename, "r");

  // check if file exists
  if (!fd) {
    return false;
  }

  fclose(fd);
  return true;
}

/*
  ┌───────────────────────────────┐
  │   FOLDER EXISTS?              │
  └───────────────────────────────┘
 */

bool folder_exists(FILE *fd, char *foldername) {
  if (!foldername) {
    return false;
  }

  fd = fopen(foldername, "r");

  // check if file exists
  if (!fd) {
    return false;
  }

  return true;
}

/*
  ┌───────────────────────────────┐
  │   PRINTING                    │
  └───────────────────────────────┘
 */

void print_file(FILE *fd, char *filename) {
  if (!filename) {
    printf("Error opening file!\n");
    return;
  }

  fd = open(filename, O_RDONLY);

  size_t mmapLen = file_size(fd, filename);
  off_t offset = 0; // offset to seek to.

  // We use MAP_PRIVATE since writes to the region of memory should
  // not be written back to the file.
  char *ptr = (char *)mmap(NULL, mmapLen, PROT_READ, MAP_PRIVATE, fd, offset);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // File isn't necessarily loaded into memory until we access the
  // memory (causing a page fault to occur and making the kernel
  // handle it).
  printf("%s\n", ptr);

  close(fd);
}
