static PRStatus nspr_io_close(PRFileDesc *fd)
{
  const PRCloseFN close_fn = PR_GetDefaultIOMethods()->close;
  fd->secret = NULL;
  return close_fn(fd);
}
