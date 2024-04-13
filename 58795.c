static int robots_fsio_close(pr_fh_t *fh, int fd) {
  if (fd != AUTH_ROBOTS_TXT_FD) {
    errno = EINVAL;
    return -1;
  }

  return 0;
}
