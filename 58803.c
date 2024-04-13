static int robots_fsio_write(pr_fh_t *fh, int fd, const char *buf,
    size_t bufsz) {
  if (fd != AUTH_ROBOTS_TXT_FD) {
    errno = EINVAL;
    return -1;
  }

  return (int) bufsz;
}
