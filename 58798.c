static int robots_fsio_lstat(pr_fs_t *fs, const char *path, struct stat *st) {
  return robots_fsio_stat(fs, path, st);
}
