#ifndef _CKSFV_H_
#define _CKSFV_H_

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"

#ifndef PATH_MAX
#define PATH_MAX (2048)
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE (0)
#endif


extern int use_basename;
extern int be_quiet;
extern int be_caseinsensitive;
extern int be_backslashinsensitive;
extern char **sfv_broken_list;
extern int sfv_broken;
extern FILE *progress_file;

int readsfv(char *filename, char *dir, int argc, char **argv);
int recursivereadsfv(char *dir, int follow, int argc, char **argv);
int newsfv(char **);
void pusage(void);

void pnsfv_head();
void pfileinfo(char **);
void pcrc(char *fn, uint32_t val);
int crc32(int fd, uint32_t * val);
void prsfv_head(char *fn);

#define QUIET (be_quiet >= 1)
#define TOTALLY_QUIET (be_quiet >= 2)

#endif
