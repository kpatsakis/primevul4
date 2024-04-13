CURLcode Curl_nss_connect(struct connectdata *conn, int sockindex)
{
  return nss_connect_common(conn, sockindex, /* blocking */ NULL);
}
