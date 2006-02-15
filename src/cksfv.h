#ifndef _CKSFV_H_
#define _CKSFV_H_

#include "config.h"

extern int use_basename;
extern int be_quiet;
extern int be_caseinsensitive;
extern int be_backslashinsensitive;
extern int recursive;
extern char **sfv_broken_list;
extern int sfv_broken;

int readsfv(char *filename, char *dir, int argc, char **argv);
int recursivereadsfv(char *dir, int follow, int argc, char **argv);
int newsfv(char **);
void pusage(void);

void pnsfv_head();
void pfileinfo(char **);
void pcrc(char *fn, uint32_t val);
int crc32(int fd, uint32_t *val);
void prsfv_head(char *fn);

#define QUIET (be_quiet >= 1)
#define TOTALLY_QUIET (be_quiet >= 2)

#endif
