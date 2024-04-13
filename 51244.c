void Curl_nss_close(struct connectdata *conn, int sockindex)
{
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];

  if(connssl->handle) {
    /* NSS closes the socket we previously handed to it, so we must mark it
       as closed to avoid double close */
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;

    if((connssl->client_nickname != NULL) || (connssl->obj_clicert != NULL))
      /* A server might require different authentication based on the
       * particular path being requested by the client.  To support this
       * scenario, we must ensure that a connection will never reuse the
       * authentication data from a previous connection. */
      SSL_InvalidateSession(connssl->handle);

    free(connssl->client_nickname);
    connssl->client_nickname = NULL;
    /* destroy all NSS objects in order to avoid failure of NSS shutdown */
    Curl_llist_destroy(connssl->obj_list, NULL);
    connssl->obj_list = NULL;
    connssl->obj_clicert = NULL;

    PR_Close(connssl->handle);
    connssl->handle = NULL;
  }
}
