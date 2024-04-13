static ssize_t nss_recv(struct connectdata * conn, /* connection data */
                        int num,                   /* socketindex */
                        char *buf,                 /* store read data here */
                        size_t buffersize,         /* max amount to read */
                        CURLcode *curlcode)
{
  ssize_t nread = PR_Recv(conn->ssl[num].handle, buf, (int)buffersize, 0,
                          PR_INTERVAL_NO_WAIT);
  if(nread < 0) {
    /* failed SSL read */
    PRInt32 err = PR_GetError();

    if(err == PR_WOULD_BLOCK_ERROR)
      *curlcode = CURLE_AGAIN;
    else {
      /* print the error number and error string */
      const char *err_name = nss_error_to_name(err);
      infof(conn->data, "SSL read: errno %d (%s)\n", err, err_name);

      /* print a human-readable message describing the error if available */
      nss_print_error_message(conn->data, err);

      *curlcode = (is_cc_error(err))
        ? CURLE_SSL_CERTPROBLEM
        : CURLE_RECV_ERROR;
    }

    return -1;
  }

  return nread;
}
