static CURLcode display_conn_info(struct connectdata *conn, PRFileDesc *sock)
{
  CURLcode result = CURLE_OK;
  SSLChannelInfo channel;
  SSLCipherSuiteInfo suite;
  CERTCertificate *cert;
  CERTCertificate *cert2;
  CERTCertificate *cert3;
  PRTime now;
  int i;

  if(SSL_GetChannelInfo(sock, &channel, sizeof channel) ==
     SECSuccess && channel.length == sizeof channel &&
     channel.cipherSuite) {
    if(SSL_GetCipherSuiteInfo(channel.cipherSuite,
                              &suite, sizeof suite) == SECSuccess) {
      infof(conn->data, "SSL connection using %s\n", suite.cipherSuiteName);
    }
  }

  cert = SSL_PeerCertificate(sock);
  if(cert) {
    infof(conn->data, "Server certificate:\n");

    if(!conn->data->set.ssl.certinfo) {
      display_cert_info(conn->data, cert);
      CERT_DestroyCertificate(cert);
    }
    else {
      /* Count certificates in chain. */
      now = PR_Now();
      i = 1;
      if(!cert->isRoot) {
        cert2 = CERT_FindCertIssuer(cert, now, certUsageSSLCA);
        while(cert2) {
          i++;
          if(cert2->isRoot) {
            CERT_DestroyCertificate(cert2);
            break;
          }
          cert3 = CERT_FindCertIssuer(cert2, now, certUsageSSLCA);
          CERT_DestroyCertificate(cert2);
          cert2 = cert3;
        }
      }

      result = Curl_ssl_init_certinfo(conn->data, i);
      if(!result) {
        for(i = 0; cert; cert = cert2) {
          result = Curl_extract_certinfo(conn, i++, (char *)cert->derCert.data,
                                         (char *)cert->derCert.data +
                                                 cert->derCert.len);
          if(result)
            break;

          if(cert->isRoot) {
            CERT_DestroyCertificate(cert);
            break;
          }

          cert2 = CERT_FindCertIssuer(cert, now, certUsageSSLCA);
          CERT_DestroyCertificate(cert);
        }
      }
    }
  }

  return result;
}
