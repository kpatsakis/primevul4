static void nss_update_connecting_state(ssl_connect_state state, void *secret)
{
  struct ssl_connect_data *connssl = (struct ssl_connect_data *)secret;
  if(PR_GetError() != PR_WOULD_BLOCK_ERROR)
    /* an unrelated error is passing by */
    return;

  switch(connssl->connecting_state) {
  case ssl_connect_2:
  case ssl_connect_2_reading:
  case ssl_connect_2_writing:
    break;
  default:
    /* we are not called from an SSL handshake */
    return;
  }

  /* update the state accordingly */
  connssl->connecting_state = state;
}
