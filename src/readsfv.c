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
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>

#include "cksfv.h"
#include "config.h"

#ifndef WIN32
#define O_BINARY (0)
#endif

static int find_file(char *filename, char *dir);


char **sfv_broken_list = NULL;
int sfv_broken = 0;


/* add broken sfv to the list */
static void add_broken_entry(char *fn, char *dir)
{
    char sfvname[PATH_MAX + 1];
    char **new_broken;

    sfv_broken++;
    new_broken = realloc(sfv_broken_list, sfv_broken * sizeof(char **));
    if (new_broken) {
	sfv_broken_list = new_broken;
	snprintf(sfvname, sizeof(sfvname), "%s/%s", dir, fn);
	sfv_broken_list[sfv_broken - 1] = strdup(sfvname);
    } else {
	fprintf(stderr,
		"cksfv: very annoying... no space for broken list...\n");
	sfv_broken--;
    }
}


int readsfv(char *fn, char *dir, int argc, char **argv)
{
    FILE *fd;
    char buf[PATH_MAX + 256];	/* enough for name and checksum */
    char *filename;
    char *end;
    int file, rval = 0;
    uint32_t sfvcrc;
    uint32_t val;
    int ind;
    int j;
    int check;
    struct stat st;

    if (!QUIET)
	prsfv_head(fn);

    fd = fopen(fn, "r");
    if (fd == NULL) {
	if (!TOTALLY_QUIET)
	    fprintf(stderr, "cksfv: %s: %s\n", fn, strerror(errno));
	return 1;
    }

    if (chdir(dir) != 0) {
	if (!TOTALLY_QUIET)
	    fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
	goto error;
    }

    while (fgets(buf, sizeof(buf), fd)) {
	/* comment in the sfv file ignore */
	if (buf[0] == ';' || buf[0] == '\n' || buf[0] == '\r')
	    continue;

	/* build filename and crc from the sfv file */
	if ((end = strrchr(buf, ' ')) == NULL) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: %s: incorrect sfv file format\n",
			fn);
	    goto error;
	}
	ind = ((intptr_t) end) - ((intptr_t) buf);
	if (ind >= PATH_MAX) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: too long a name\n");
	    goto error;
	}
	if ((ind + 9) >= ((intptr_t) strlen(buf))) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr,
			"cksfv: too short a line (checksum missing)\n");
	    goto error;
	}
	/* check that it's exactly 8 hexadigits */
	for (j = 1; j < 9; j++) {
	    if (!isxdigit(((int) buf[ind + j]))) {
		if (!TOTALLY_QUIET)
		    fprintf(stderr,
			    "cksfv: illegal checksum (should only contain hexdigits): %s\n",
			    &buf[ind + 1]);
		goto error;
	    }
	}
	/* must be followed by a whitespace char */
	if (!isspace((int) buf[ind + 9])) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: too long a checksum: %s\n",
			&buf[ind + 1]);
	    goto error;
	}

	buf[ind] = '\0';	/* zero between filename and checksum */
	buf[ind + 9] = '\0';	/* zero after checksum */
	sfvcrc = strtoul(&buf[ind + 1], NULL, 16);

	filename = buf;

	if (argc) {
	    check = 0;
	    for (j = 0; j < argc; j++) {
		if (argv[j] == NULL)
		    continue;
		if ((be_caseinsensitive != 0
		     && strcasecmp(argv[j], filename) == 0)
		    || (be_caseinsensitive == 0
			&& strcmp(argv[j], filename) == 0)) {
		    check = 1;
		    argv[j] = NULL;
		    break;
		}
	    }
	    if (check == 0)
		continue;
	}

	if (strlen(filename) >= PATH_MAX) {
	    if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: filename too long\n");
	    rval = 1;
	    continue;
	}

	/* Replace backslashes with slashes */
	if (be_backslashinsensitive == 1) {
	    j = 0;
	    for (j = 0; filename[j] != 0; j++) {
		if (filename[j] == '\\')
		    filename[j] = '/';
	    }
	}

	if (!QUIET) {
	    fprintf(progress_file, "%-49s ", filename);
	    fflush(progress_file);
	}

	/* can we open the file */
	if ((file =
	     open(filename, O_RDONLY | O_LARGEFILE | O_BINARY, 0)) < 0) {
	    if (be_caseinsensitive == 1) {
		/* try to search for it if ingore case is set */
		find_file(filename, dir);
		file =
		    open(filename, O_RDONLY | O_LARGEFILE | O_BINARY, 0);
	    }
	}

	/* if the file could not be opened */
	if (file < 0) {
	    if (!QUIET)
		fprintf(stderr, "%s\n", strerror(errno));
	    else if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: %s: %s\n", filename,
			strerror(errno));
	    rval = 1;
	    continue;
	}

	if (fstat(file, &st)) {
	    if (!QUIET) {
		fprintf(stderr, "can not fstat\n");
	    } else if (!TOTALLY_QUIET) {
		fprintf(stderr, "cksfv: can not fstat %s: %s\n", filename,
			strerror(errno));
	    }
	    rval = 1;
	    goto next;
	}
	if (S_ISDIR(st.st_mode)) {
	    if (!QUIET) {
		fprintf(stderr, "Is a directory\n");
	    } else if (!TOTALLY_QUIET) {
		fprintf(stderr, "cksfv: %s: Is a directory\n", filename);
	    }
	    rval = 1;
	    goto next;
	}

	if (crc32(file, &val)) {
	    /* file error */
	    if (!QUIET)
		fprintf(stderr, "%s\n", strerror(errno));
	    else if (!TOTALLY_QUIET)
		fprintf(stderr, "cksfv: %s: %s\n", filename,
			strerror(errno));
	    rval = 1;
	} else {
	    if (val != sfvcrc) {
		if (!QUIET) {
		    fprintf(progress_file, "different CRC\n");
		    fflush(progress_file);
		} else if (!TOTALLY_QUIET)
		    fprintf(stderr, "cksfv: %s: Has a different CRC\n",
			    filename);
		rval = 1;
	    } else if (!QUIET) {
		fprintf(progress_file, "OK\n");
		fflush(progress_file);
	    }
	}
      next:
	close(file);
    }

    fclose(fd);

    if (argc) {
	for (j = 0; j < argc; j++) {
	    if (argv[j]) {
		if (!QUIET) {
		    fprintf(stderr, "%-49s not found in sfv\n", argv[j]);
		} else if (!TOTALLY_QUIET) {
		    fprintf(stderr, "cksfv: %s: not found in sfv\n",
			    argv[j]);
		}
		rval = 1;
	    }
	}
    }

    if (!QUIET) {
	fprintf(stderr,
		"--------------------------------------------------------------------------------\n");
	fprintf(progress_file, "%s\a\n",
		rval == 0 ? "Everything OK" : "Errors Occured");
	fflush(progress_file);
    }

    if (rval)
	add_broken_entry(fn, dir);

    return rval;

  error:
    fclose(fd);
    add_broken_entry(fn, dir);
    return 1;
}


static int find_file(char *filename, char *dir)
{
    DIR *dirp;
    struct dirent *dirinfo;
    char *foo;
    char *bar;

    dirp = opendir(".");
    if (dirp == NULL) {
	if (!QUIET) {
	    fprintf(stderr, "%s", strerror(errno));
	} else if (!TOTALLY_QUIET) {
	    fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
	}
	return 0;
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


int recursivereadsfv(char *dir, int follow, int argc, char **argv)
{
    DIR *dirp;
    struct dirent *dirinfo;
    char cwd[PATH_MAX + 1];
    struct stat dirstat;
    int ret;
    int finalret = 0;

    if (!getcwd(cwd, sizeof(cwd))) {
	if (!QUIET) {
	    fprintf(stderr, "getcwd:\n");
	} else if (!TOTALLY_QUIET) {
	    fprintf(stderr, "getcwd: %s\n", strerror(errno));
	}
	return 1;
    }

    if (dir && strcmp(dir, ".") != 0) {
	if (chdir(dir) == -1) {
	    if (!QUIET) {
		fprintf(stderr, "chdir:\n");
	    } else if (!TOTALLY_QUIET) {
		fprintf(stderr, "cksfv: cannot chdir to %s: %s\n", dir,
			strerror(errno));
	    }
	    return 1;
	}
    }

    dirp = opendir(".");

    if (dirp == NULL) {
	if (!QUIET) {
	    fprintf(stderr, "%s", strerror(errno));
	} else if (!TOTALLY_QUIET) {
	    fprintf(stderr, "cksfv: %s: %s\n", dir, strerror(errno));
	}
	return 1;
    }

    while ((dirinfo = readdir(dirp)) != NULL) {

	if (strcmp(dirinfo->d_name, ".") == 0)
	    continue;

	if (strcmp(dirinfo->d_name, "..") == 0)
	    continue;

#ifndef WIN32
	if (!follow)
	    ret = lstat(dirinfo->d_name, &dirstat);
	else
#endif
	    ret = stat(dirinfo->d_name, &dirstat);

	if (ret == -1) {
	    if (!QUIET) {
		fprintf(stderr, "cannot fstat\n");
	    } else if (!TOTALLY_QUIET) {
		fprintf(stderr, "cksfv: cannot stat %s: %s\n",
			dirinfo->d_name, strerror(errno));
	    }
	    closedir(dirp);
	    return 1;
	}

	if (S_ISDIR(dirstat.st_mode)) {
	    /* Recursive descent into a directory */
	    if (recursivereadsfv(dirinfo->d_name, follow, argc, argv))
		finalret = 1;

	} else if (S_ISREG(dirstat.st_mode) &&
		   (strcasecmp
		    (dirinfo->d_name + strlen(dirinfo->d_name) - 4,
		     ".sfv") == 0)) {
	    /* Read an sfv file */
	    char processdir[PATH_MAX];

	    if (!getcwd(processdir, sizeof(processdir))) {

		if (!QUIET) {
		    fprintf(stderr, "getcwd:\n");
		} else if (!TOTALLY_QUIET) {
		    fprintf(stderr, "getcwd: %s\n", strerror(errno));
		}

		closedir(dirp);

		return 1;
	    }

	    if (!QUIET)
		fprintf(stderr, "Entering directory: %s\n", processdir);

	    if (readsfv(dirinfo->d_name, processdir, argc, argv))
		finalret = 1;
	}
    }

    if (chdir(cwd) == -1) {
	if (!QUIET) {
	    fprintf(stderr, "chdir:\n");
	} else if (!TOTALLY_QUIET) {
	    fprintf(stderr, "cksfv: cannot chdir to %s: %s\n", cwd,
		    strerror(errno));
	}
	return 1;
    }

    closedir(dirp);

    return finalret;
}
