/* readsfv.c - reads from an existing checksum listing and verifies that
   the files are good
   
   Copyright (C) 2000 Bryan Call <bc@fodder.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#ifndef PATH_MAX
#define PATH_MAX (512)
#endif

extern int crc32(int fd, uint32_t *val, uint64_t *len);
extern void prsfv_head(char *fn);

static int find_file(char *filename, char *dir, int quiet);


int readsfv(char *fn, char *dir, int nocase, int quiet)
{
  FILE *fd;
  char buf[PATH_MAX + 256]; /* enough for name and checksum */
  char *filename;
  char *end;
  int file, rval = 0;
  uint64_t len;
  uint32_t sfvcrc;
  uint32_t val;
  int ind;
  int j;

  if (quiet == 0)
    prsfv_head(fn);

  fd = fopen(fn, "r");
  if (fd == NULL) {
    if (quiet != 2)
      fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno));
    exit(1);
  }

  if (chdir(dir) != 0) {
    if (quiet != 2)
      fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
    exit(1);
  }

  while (fgets(buf, sizeof(buf), fd)) {
    /* comment in the sfv file ignore */
    if (buf[0] == ';' || buf[0] == '\n' || buf[0] == '\r')
      continue;

    /* build filename and crc from the sfv file */
    if ((end = strrchr(buf, ' ')) == NULL) {
      fprintf(stderr, "cksfv: %s: incorrect sfv file format\n", fn);
      exit(1);
    }
    ind = ((intptr_t) end) - ((intptr_t) buf);
    if (ind >= PATH_MAX) {
      fprintf(stderr, "cksfv: too long a name\n");
      exit(1);
    }
    if ((ind + 9) >= ((intptr_t) strlen(buf))) {
      fprintf(stderr, "cksfv: too short a line (checksum missing)\n");
      exit(1);
    }
    /* check that it's exactly 8 hexadigits */
    for (j = 1; j < 9; j++) {
      if (!isxdigit(((int) buf[ind + j]))) {
	fprintf(stderr, "cksfv: illegal checksum (should only contain hexdigits): %s\n", &buf[ind + 1]);
	exit(1);
      }
    }
    buf[ind] = '\0'; /* zero between filename and checksum */
    buf[ind + 9] = '\0'; /* zero after checksum */
    sfvcrc = strtoul(&buf[ind + 1], NULL, 16);

    filename = buf;
    if (strlen(filename) >= PATH_MAX) {
      fprintf(stderr, "cksfv: filename too long\n");
      exit(1);
    }

    if (quiet == 0)
      fprintf(stderr, "%-49s ", filename);

    /* can we open the file */
    if ((file = open(filename, O_RDONLY | O_LARGEFILE, 0)) < 0) {
      if (nocase == 1) {
	/* try to search for it if ingore case is set */
	find_file(filename, dir, quiet);
	file = open(filename, O_RDONLY | O_LARGEFILE, 0);
      }
    }

    /* if the file could not be opened */
    if (file < 0) {
      if (quiet == 0)
	fprintf(stderr, "%s\n", strerror(errno));
      else if (quiet == 1)
	fprintf(stderr, "cksfv: %s: %s\n", filename, strerror(errno));
      rval = 1;
      continue;
    }

    if (crc32(file, &val, &len)) {
      if (quiet == 0)
	fprintf(stderr, "%s\n", strerror(errno));
      else if (quiet == 1)
	fprintf(stderr, "cksfv: %s: %s\n", filename, strerror(errno));
      rval = 1;
    } else {
      if (val != sfvcrc) {
	if (quiet == 0)
	  fprintf(stderr, "different CRC\n");
	else if (quiet == 1)
	  fprintf(stderr, "cksfv: %s: Has a different CRC\n", filename);
	rval = 1;
      } else
	if (quiet == 0)
	  fprintf(stderr, "OK\n");
    }
    close(file);
  }
  fclose(fd);

  if (quiet == 0) {
    if (rval == 0) {
      printf("--------------------------------------------------------------------------------\nEverything OK\a\n");
    } else {
      printf("--------------------------------------------------------------------------------\nErrors Occured\a\n");
    }
  }
  return rval;
}


static int find_file(char *filename, char *dir, int quiet)
{
  DIR *dirp;
  struct dirent *dirinfo;
  char *foo;
  char *bar;

  dirp = opendir(".");
  if (dirp == NULL) {
    if (quiet != 2)
      fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
    return(0);
  }

  while ((dirinfo = readdir(dirp)) != NULL) {
    for (foo = filename, bar = dirinfo->d_name; *foo != '\0' ||
           *bar != '\0'; foo++, bar++) {
      if (*foo != *bar) {
        if (isalpha((int) *foo) && isalpha((int) *bar)) {
          if (tolower((int) *foo) != tolower((int) *bar)) {
            break;
          }
        } else {
          if (!((*foo == '_' || *foo == ' ') &&
                (*bar == '_' || *bar == ' '))) {
            break;
          }
        }
      }
    }
    if (*foo == '\0' && *bar == '\0')
      strcpy(filename, dirinfo->d_name);
      
  }
  rewinddir(dirp);
  return 1;
}
