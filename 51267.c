static SECStatus nss_auth_cert_hook(void *arg, PRFileDesc *fd, PRBool checksig,
                                    PRBool isServer)
{
  struct connectdata *conn = (struct connectdata *)arg;

#ifdef SSL_ENABLE_OCSP_STAPLING
  if(conn->data->set.ssl.verifystatus) {
    SECStatus cacheResult;

    const SECItemArray *csa = SSL_PeerStapledOCSPResponses(fd);
    if(!csa) {
      failf(conn->data, "Invalid OCSP response");
      return SECFailure;
    }

    if(csa->len == 0) {
      failf(conn->data, "No OCSP response received");
      return SECFailure;
    }

    cacheResult = CERT_CacheOCSPResponseFromSideChannel(
      CERT_GetDefaultCertDB(), SSL_PeerCertificate(fd),
      PR_Now(), &csa->items[0], arg
    );

    if(cacheResult != SECSuccess) {
      failf(conn->data, "Invalid OCSP response");
      return cacheResult;
    }
  }
#endif

  if(!conn->data->set.ssl.verifypeer) {
    infof(conn->data, "skipping SSL peer certificate verification\n");
    return SECSuccess;
  }

  return SSL_AuthCertificate(CERT_GetDefaultCertDB(), fd, checksig, isServer);
}
