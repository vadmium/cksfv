/* newsfv.c - creates a new checksum listing
   
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

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

extern void pnsfv_head();
extern void pfileinfo(char**);
extern void pcrc(char *fn, uint32_t val);
extern int crc32(int fd, uint32_t *val, uint64_t *len);

int newsfv(char **argv)
{
  int fd, rval = 0;
  char *fn;
  uint64_t len;
  uint32_t val;

  pnsfv_head();
  pfileinfo(argv);

  while (*argv) {
    fn = *argv++;
    if ((fd = open(fn, O_RDONLY | O_LARGEFILE, 0)) < 0) {
      fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno)); 
      rval = 1;
      continue;
    }

    if (crc32(fd, &val, &len)) {
      fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno)); 
      rval = 1;
    } else
      pcrc(fn, (unsigned long) val);
    close(fd);
  }

  return rval;
}
