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

extern int  readsfv(char*, char*, int);
extern int  newsfv(char**);
extern void pusage();

int quiet = 0; /* 1 if you want the program to be quiet by default */

int main(int argc, char *argv[])
{
  int   ch, rval;
  int   nocase = 0, rsfvflag = 0;
  char  dir[256] = ".", sfvfile[256];

  while ((ch = getopt(argc, argv, "iC:f:qv")) != -1)
    switch (ch) {
    case 'i':
      nocase = 1;
      break;
    case 'C':
      strncpy(dir, optarg, 256);
      break;  
    case 'f':
      strncpy(sfvfile, optarg, 256);
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
    rval = readsfv(sfvfile, dir, nocase);
  } else {
    if (argc < 1) {
      pusage();
    }

    rval = newsfv(argv);
  }

  exit(rval);
}
