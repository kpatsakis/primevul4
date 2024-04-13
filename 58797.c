static int robots_fsio_fstat(pr_fh_t *fh, int fd, struct stat *st) {
  if (fd != AUTH_ROBOTS_TXT_FD) {
    errno = EINVAL;
    return -1;
  }

  return robots_fsio_stat(NULL, NULL, st);
}
