static int robots_fsio_stat(pr_fs_t *fs, const char *path, struct stat *st) {
  st->st_dev = (dev_t) 0;
  st->st_ino = (ino_t) 0;
  st->st_mode = (S_IFREG|S_IRUSR|S_IRGRP|S_IROTH);
  st->st_nlink = 0;
  st->st_uid = (uid_t) 0;
  st->st_gid = (gid_t) 0;
  st->st_atime = 0;
  st->st_mtime = 0;
  st->st_ctime = 0;
  st->st_size = strlen(AUTH_ROBOTS_TXT);
  st->st_blksize = 1024;
  st->st_blocks = 1;

  return 0;
}
