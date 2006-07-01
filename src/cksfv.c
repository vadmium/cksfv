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

#include "cksfv.h"

#ifndef PATH_MAX
#define PATH_MAX (512)
#endif

/* config variables for other modules */
int use_basename = 0;
int be_quiet = 0;
int be_caseinsensitive = 0;
int be_backslashinsensitive = 0;
int recurse = 0;

int main(int argc, char *argv[])
{
  int   ch, rval;
  int   rsfvflag = 0;
  char  dir[PATH_MAX + 1] = ".", sfvfile[PATH_MAX + 1];
  int follow = 0;
  int i;

  while ((ch = getopt(argc, argv, "iC:f:qvbrLs")) != -1)
    switch (ch) {
    case 'i':
      be_caseinsensitive = 1;
      break;
    case 's':
      be_backslashinsensitive = 1;
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
    case 'L':
#ifndef WIN32
      follow = 1;
#else
      fprintf(stderr, "cksfv: ignoring -L in Windows\n");
#endif
      break;
    case 'q':
      be_quiet++;
      break;
    case 'v':
      be_quiet = 0;
      break;
    case 'b':
      use_basename = 1;
      break;
    case 'r':
      recurse = 1;
      rsfvflag = 1;
      break;
    case '?':
    default:
      pusage();
    }
  argc -= optind;
  argv += optind;

  if (rsfvflag == 1) {
    if (recurse == 1)
      rval = recursivereadsfv(dir, follow, argc, argv);
    else
      rval = readsfv(sfvfile, dir, argc, argv);
  } else {
    if (argc < 1) {
      pusage();
    }

    rval = newsfv(argv);
  }

  if (!TOTALLY_QUIET && recurse && sfv_broken) {
    fprintf(stderr, "\nList of sfv files with broken files (or broken sfv files):\n");
    for (i = 0; i < sfv_broken; i++)
      fprintf(stderr, "%s\n", sfv_broken_list[i]);
  }
  exit(rval);
}
