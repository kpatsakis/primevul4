static int robots_fsio_open(pr_fh_t *fh, const char *path, int flags) {
  if (flags != O_RDONLY) {
    errno = EINVAL;
    return -1;
  }

  return AUTH_ROBOTS_TXT_FD;
}
