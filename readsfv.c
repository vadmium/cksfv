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

extern int  crc32(int, unsigned long*, unsigned long*);

void strlower(char*);

int readsfv(char *fn, char *dir, int nocase)
{
  FILE          *fd;
  DIR           *dirp = NULL;
  char          buf[512], *end, filename[512], crc[9], path[256];
  int           file, rval = 0;
  unsigned long len, val, sfvcrc;
  struct dirent *dirinfo;
  
  fd = fopen(fn, "r");
  if (fd == NULL) {
    fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno));
    exit(1);
  }

  if (chdir(dir) != 0) {
    fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
    exit(1);
  }
  
  /* if case insensitive is choosen */
  if (nocase == 1) {
    dirp = opendir(".");
    if (dirp == NULL) {
      fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
      exit(1);
    }
  }
  
  while(fgets(buf, 512, fd)) {
    if (buf[0] != ';') {                /* comment in the sfv file ignore */
      /* build filename and crc from the sfv file */
      end = strrchr(buf, ' ');
      if (end == NULL) {
        fprintf(stderr, "cksfv: %s: incorrect sfv file format\n", fn);
      }
      *end = '\0';
      *(end+9) = '\0';
      strncpy(crc, ++end, 9);
      strncpy(filename, buf, 512);
      sfvcrc = strtoul(crc, '\0', 16);

      if (nocase == 1) {
        len = strlen(filename);
        while ((dirinfo = readdir(dirp)) != NULL) {
          if (!strcasecmp(dirinfo->d_name, filename)) {
            strncpy(filename, dirinfo->d_name, 512);
            break;
          }
        }
        rewinddir(dirp);
      }
      
      snprintf(path, 256, "%s/%s", dir, filename);
      
      if ((file = open(filename, O_RDONLY, 0)) < 0) {
        fprintf(stderr, "cksfv: %s: %s\n", filename, strerror(errno));
        rval = 1;
        continue;
      }
      
      if (crc32(file, &val, &len)) 
        fprintf(stderr, "cksfv: %s: %s\n", filename, strerror(errno));

      if (val != sfvcrc) {
        fprintf(stderr, "cksfv: %s: Has a different CRC\n", filename);
        rval = 1;
      }
      close(file);
    }
  }
  fclose(fd);

  return(rval);
}
