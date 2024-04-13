static int robots_fsio_read(pr_fh_t *fh, int fd, char *buf, size_t bufsz) {
  size_t robots_len;

  if (fd != AUTH_ROBOTS_TXT_FD) {
    errno = EINVAL;
    return -1;
  }

  robots_len = strlen(AUTH_ROBOTS_TXT);

  if (bufsz < robots_len) {
    errno = EINVAL;
    return -1;
  }

  memcpy(buf, AUTH_ROBOTS_TXT, robots_len);
  return (int) robots_len;
}
