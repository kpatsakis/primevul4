CURLcode Curl_nss_connect_nonblocking(struct connectdata *conn,
                                      int sockindex, bool *done)
{
  return nss_connect_common(conn, sockindex, done);
}
