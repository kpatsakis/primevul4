static PRInt32 nspr_io_recv(PRFileDesc *fd, void *buf, PRInt32 amount,
                            PRIntn flags, PRIntervalTime timeout)
{
  const PRRecvFN recv_fn = fd->lower->methods->recv;
  const PRInt32 rv = recv_fn(fd->lower, buf, amount, flags, timeout);
  if(rv < 0)
    /* check for PR_WOULD_BLOCK_ERROR and update blocking direction */
    nss_update_connecting_state(ssl_connect_2_reading, fd->secret);
  return rv;
}
