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

#include <stdint.h>
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

extern int  crc32(int, uint32_t *, unsigned long*);
extern void prsfv_head(char*);

int find_file(char*, char*);

extern int  quiet;
int readsfv(char *fn, char *dir, int nocase)
{
  FILE          *fd;
  char          buf[512], *end, filename[512], crc[9], path[256];
  int           file, rval = 0;
  unsigned long len, sfvcrc;
  uint32_t val;

  
  if (quiet == 0) {
    prsfv_head(fn);
  }
  
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
  
  while(fgets(buf, 512, fd)) {
    /* comment in the sfv file ignore */
    if (buf[0] != ';' && buf[0] != '\n' && buf[0] != '\r') { 
      /* build filename and crc from the sfv file */
      end = strrchr(buf, ' ');
      if (end == NULL) {
        fprintf(stderr, "cksfv: %s: incorrect sfv file format\n", fn);
        exit(1);
      }
      *end = '\0';
      *(end+9) = '\0';
      strncpy(crc, ++end, 9);
      strncpy(filename, buf, 512);
      sfvcrc = strtoul(crc, '\0', 16);

      if (quiet == 0)
        fprintf(stderr, "%-49s ", filename);
      
      snprintf(path, 256, "%s/%s", dir, filename);

      /* can we open the file */
      if ((file = open(filename, O_RDONLY, 0)) < 0) {
        if (nocase == 1) {
          /* try to search for it if ingore case is set */
          find_file(filename, dir);
          file = open(filename, O_RDONLY, 0);
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
        if (((unsigned long) val) != sfvcrc) {
          if (quiet == 0)
            fprintf(stderr, "different CRC\n");
          else if (quiet == 1)
            fprintf(stderr, "cksfv: %s: Has a different CRC\n", filename);
          rval = 1;
        } else
          if (quiet == 0) {
            fprintf(stderr, "OK\n");
          }
      }
      close(file);
    }
  }
  fclose(fd);

  if (quiet == 0) {
    if (rval == 0) {
      printf("--------------------------------------------------------------------------------\nEverything OK\a\n");
    } else {
      printf("--------------------------------------------------------------------------------\nErrors Occured\a\n");
    }
  }

  return(rval);
}


int find_file(char* filename, char* dir)
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
        if (isalpha(*foo) && isalpha(*bar)) {
          if (tolower(*foo) != tolower(*bar)) {
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
