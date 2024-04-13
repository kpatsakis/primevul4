static void HandshakeCallback(PRFileDesc *sock, void *arg)
{
  struct connectdata *conn = (struct connectdata*) arg;
  unsigned int buflenmax = 50;
  unsigned char buf[50];
  unsigned int buflen;
  SSLNextProtoState state;

  if(!conn->bits.tls_enable_npn && !conn->bits.tls_enable_alpn) {
    return;
  }

  if(SSL_GetNextProto(sock, &state, buf, &buflen, buflenmax) == SECSuccess) {

    switch(state) {
    case SSL_NEXT_PROTO_NO_SUPPORT:
    case SSL_NEXT_PROTO_NO_OVERLAP:
      infof(conn->data, "ALPN/NPN, server did not agree to a protocol\n");
      return;
#ifdef SSL_ENABLE_ALPN
    case SSL_NEXT_PROTO_SELECTED:
      infof(conn->data, "ALPN, server accepted to use %.*s\n", buflen, buf);
      break;
#endif
    case SSL_NEXT_PROTO_NEGOTIATED:
      infof(conn->data, "NPN, server accepted to use %.*s\n", buflen, buf);
      break;
    }

#ifdef USE_NGHTTP2
    if(buflen == NGHTTP2_PROTO_VERSION_ID_LEN &&
       !memcmp(NGHTTP2_PROTO_VERSION_ID, buf, NGHTTP2_PROTO_VERSION_ID_LEN)) {
      conn->negnpn = CURL_HTTP_VERSION_2;
    }
    else
#endif
    if(buflen == ALPN_HTTP_1_1_LENGTH &&
       !memcmp(ALPN_HTTP_1_1, buf, ALPN_HTTP_1_1_LENGTH)) {
      conn->negnpn = CURL_HTTP_VERSION_1_1;
    }
  }
}
