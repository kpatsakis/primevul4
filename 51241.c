static SECStatus CanFalseStartCallback(PRFileDesc *sock, void *client_data,
                                       PRBool *canFalseStart)
{
  struct connectdata *conn = client_data;
  struct Curl_easy *data = conn->data;

  SSLChannelInfo channelInfo;
  SSLCipherSuiteInfo cipherInfo;

  SECStatus rv;
  PRBool negotiatedExtension;

  *canFalseStart = PR_FALSE;

  if(SSL_GetChannelInfo(sock, &channelInfo, sizeof(channelInfo)) != SECSuccess)
    return SECFailure;

  if(SSL_GetCipherSuiteInfo(channelInfo.cipherSuite, &cipherInfo,
                            sizeof(cipherInfo)) != SECSuccess)
    return SECFailure;

  /* Prevent version downgrade attacks from TLS 1.2, and avoid False Start for
   * TLS 1.3 and later. See https://bugzilla.mozilla.org/show_bug.cgi?id=861310
   */
  if(channelInfo.protocolVersion != SSL_LIBRARY_VERSION_TLS_1_2)
    goto end;

  /* Only allow ECDHE key exchange algorithm.
   * See https://bugzilla.mozilla.org/show_bug.cgi?id=952863 */
  if(cipherInfo.keaType != ssl_kea_ecdh)
    goto end;

  /* Prevent downgrade attacks on the symmetric cipher. We do not allow CBC
   * mode due to BEAST, POODLE, and other attacks on the MAC-then-Encrypt
   * design. See https://bugzilla.mozilla.org/show_bug.cgi?id=1109766 */
  if(cipherInfo.symCipher != ssl_calg_aes_gcm)
    goto end;

  /* Enforce ALPN or NPN to do False Start, as an indicator of server
   * compatibility. */
  rv = SSL_HandshakeNegotiatedExtension(sock, ssl_app_layer_protocol_xtn,
                                        &negotiatedExtension);
  if(rv != SECSuccess || !negotiatedExtension) {
    rv = SSL_HandshakeNegotiatedExtension(sock, ssl_next_proto_nego_xtn,
                                          &negotiatedExtension);
  }

  if(rv != SECSuccess || !negotiatedExtension)
    goto end;

  *canFalseStart = PR_TRUE;

  infof(data, "Trying TLS False Start\n");

end:
  return SECSuccess;
}
