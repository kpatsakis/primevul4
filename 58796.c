static int robots_fsio_faccess(pr_fh_t *fh, int mode, uid_t uid, gid_t gid,
    array_header *suppl_gids) {

  if (fh->fh_fd != AUTH_ROBOTS_TXT_FD) {
    errno = EINVAL;
    return -1;
  }

  if (mode != R_OK) {
    errno = EACCES;
    return -1;
  }

  return 0;
}
