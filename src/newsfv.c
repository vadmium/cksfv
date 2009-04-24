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

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>

#include "cksfv.h"
#include "config.h"


#ifndef WIN32
#define O_BINARY (0)
#endif

static int processfile(int fd, const char *fn)
{
    uint32_t val;
    char *tmpname;

    if (crc32(fd, &val)) {
	if (!TOTALLY_QUIET)
	    fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno));
	return 1;
    } else {
	if (use_basename) {
	    tmpname = strdup(fn);
	    if (tmpname == NULL) {
		if (!TOTALLY_QUIET)
		    fprintf(stderr, "out of memory\n");
		exit(1);
	    }
	    pcrc(basename(tmpname), val);
	    free(tmpname);
	} else {
	    pcrc(fn, val);
	}
    }
    return 0;
}

int newsfv(char **argv, int recursive, int follow, const char *base)
{
    int fd = -1;
    int rval = 0;
    char *fn;
    struct stat st;
    DIR *dir = NULL;
    struct dirent *dirent;
    char *tmp;

    fn = malloc(PATH_MAX);
    if (fn == NULL)
	return 1;

    if (argv != NULL) {
	/* First call to newsfv(): print headers */
	pnsfv_head();
	pfileinfo(argv);
    } else {
	/* A recursive call to newsfv() */

	dir = opendir(base);
	if (dir == NULL) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: Can not read directory %s: %s\n", fn, strerror(errno));
	    return 1;
	}
    }

    while (dir != NULL || *argv) {

	fd = -1;
	if (dir == NULL) {
	    tmp = *argv++;
	    if (snprintf(fn, PATH_MAX, "%s", tmp) >= PATH_MAX)
		goto next;
	} else {
	    /* recursive call: read next directory entry */
	    dirent = readdir(dir);
	    if (dirent == NULL)
		break;

	    if (strcmp(dirent->d_name, ".") == 0 ||
		strcmp(dirent->d_name, "..") == 0)
		goto next;

	    if (snprintf(fn, PATH_MAX, "%s/%s", base, dirent->d_name) >= PATH_MAX) {
		fprintf(stderr, "cksfv: too long a name: %s/%s\n", base, dirent->d_name);
		rval = 1;
		goto next;
	    }
	}

	fd = open(fn, O_RDONLY | O_LARGEFILE | O_BINARY, 0);
	if (fd < 0) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno));
	    rval = 1;
	    goto next;
	}

	if (stat(fn, &st)) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: can not fstat %s: %s\n", fn,
			strerror(errno));
	    rval = 1;
	    goto next;
	}

	if (S_ISDIR(st.st_mode)) {
	    if (!recursive) {
		if (!TOTALLY_QUIET)
		    fprintf(stderr, "cksfv: %s: Is a directory\n", fn);
		rval = 1;
		goto next;
	    }

	    /* Do not follow symlinks, unless follow != 0 */
	    if (lstat(fn, &st)) {
		if (!TOTALLY_QUIET)
		    fprintf(stderr, "cksfv: can not lstat %s: %s\n", fn, strerror(errno));
		rval = 1;
		goto next;
	    }
	    if (!follow && S_ISLNK(st.st_mode))
		goto next;

	    if (newsfv(NULL, recursive, follow, fn))
		rval = 1;
	    goto next;
	}

	if (processfile(fd, fn))
	    rval = 1;

      next:
	if (fd >= 0) {
	    close(fd);
	    fd = -1;
	}
    }

    if (dir != NULL)
	closedir(dir);

    free(fn);

    return rval;
}
