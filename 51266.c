static PRInt32 nspr_io_send(PRFileDesc *fd, const void *buf, PRInt32 amount,
                            PRIntn flags, PRIntervalTime timeout)
{
  const PRSendFN send_fn = fd->lower->methods->send;
  const PRInt32 rv = send_fn(fd->lower, buf, amount, flags, timeout);
  if(rv < 0)
    /* check for PR_WOULD_BLOCK_ERROR and update blocking direction */
    nss_update_connecting_state(ssl_connect_2_writing, fd->secret);
  return rv;
}
