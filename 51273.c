static CURLcode nss_do_connect(struct connectdata *conn, int sockindex)
{
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];
  struct Curl_easy *data = conn->data;
  CURLcode result = CURLE_SSL_CONNECT_ERROR;
  PRUint32 timeout;

  /* check timeout situation */
  const long time_left = Curl_timeleft(data, NULL, TRUE);
  if(time_left < 0L) {
    failf(data, "timed out before SSL handshake");
    result = CURLE_OPERATION_TIMEDOUT;
    goto error;
  }

  /* Force the handshake now */
  timeout = PR_MillisecondsToInterval((PRUint32) time_left);
  if(SSL_ForceHandshakeWithTimeout(connssl->handle, timeout) != SECSuccess) {
    if(PR_GetError() == PR_WOULD_BLOCK_ERROR)
      /* blocking direction is updated by nss_update_connecting_state() */
      return CURLE_AGAIN;
    else if(conn->data->set.ssl.certverifyresult == SSL_ERROR_BAD_CERT_DOMAIN)
      result = CURLE_PEER_FAILED_VERIFICATION;
    else if(conn->data->set.ssl.certverifyresult!=0)
      result = CURLE_SSL_CACERT;
    goto error;
  }

  result = display_conn_info(conn, connssl->handle);
  if(result)
    goto error;

  if(data->set.str[STRING_SSL_ISSUERCERT]) {
    SECStatus ret = SECFailure;
    char *nickname = dup_nickname(data, STRING_SSL_ISSUERCERT);
    if(nickname) {
      /* we support only nicknames in case of STRING_SSL_ISSUERCERT for now */
      ret = check_issuer_cert(connssl->handle, nickname);
      free(nickname);
    }

    if(SECFailure == ret) {
      infof(data, "SSL certificate issuer check failed\n");
      result = CURLE_SSL_ISSUER_ERROR;
      goto error;
    }
    else {
      infof(data, "SSL certificate issuer check ok\n");
    }
  }

  result = cmp_peer_pubkey(connssl, data->set.str[STRING_SSL_PINNEDPUBLICKEY]);
  if(result)
    /* status already printed */
    goto error;

  return CURLE_OK;

error:
  return nss_fail_connect(connssl, data, result);
}
