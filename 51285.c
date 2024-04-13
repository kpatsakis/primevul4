static CURLcode nss_set_nonblock(struct ssl_connect_data *connssl,
                                 struct Curl_easy *data)
{
  static PRSocketOptionData sock_opt;
  sock_opt.option = PR_SockOpt_Nonblocking;
  sock_opt.value.non_blocking = PR_TRUE;

  if(PR_SetSocketOption(connssl->handle, &sock_opt) != PR_SUCCESS)
    return nss_fail_connect(connssl, data, CURLE_SSL_CONNECT_ERROR);

  return CURLE_OK;
}
