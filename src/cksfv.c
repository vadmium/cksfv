/* cksfv (Check SFV) - This program is can create sfv (checksum) listings and
   test already created sfv files.
   
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

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX (512)
#endif

extern int  readsfv(char *filename, char *dir, int nocase, int quiet);
extern int  newsfv(char **);
extern void pusage();

int main(int argc, char *argv[])
{
  int   ch, rval;
  int   nocase = 0, rsfvflag = 0;
  char  dir[PATH_MAX + 1] = ".", sfvfile[PATH_MAX + 1];
  int   quiet = 0;

  while ((ch = getopt(argc, argv, "iC:f:qv")) != -1)
    switch (ch) {
    case 'i':
      nocase = 1;
      break;
    case 'C':
      strncpy(dir, optarg, sizeof(dir));
      dir[sizeof(dir) - 1] = 0;
      break;  
    case 'f':
      strncpy(sfvfile, optarg, sizeof(sfvfile));
      sfvfile[sizeof(sfvfile) - 1] = 0;
      rsfvflag = 1;
      break;
    case 'q':
      quiet++;
      break;
    case 'v':
      quiet = 0;
      break;
    case '?':
    default:
      pusage();
    }
  argc -= optind;
  argv += optind;

  if (rsfvflag == 1) {
    rval = readsfv(sfvfile, dir, nocase, quiet);
  } else {
    if (argc < 1) {
      pusage();
    }

    rval = newsfv(argv);
  }

  exit(rval);
}
