static SECStatus check_issuer_cert(PRFileDesc *sock,
                                   char *issuer_nickname)
{
  CERTCertificate *cert, *cert_issuer, *issuer;
  SECStatus res=SECSuccess;
  void *proto_win = NULL;

  cert = SSL_PeerCertificate(sock);
  cert_issuer = CERT_FindCertIssuer(cert, PR_Now(), certUsageObjectSigner);

  proto_win = SSL_RevealPinArg(sock);
  issuer = PK11_FindCertFromNickname(issuer_nickname, proto_win);

  if((!cert_issuer) || (!issuer))
    res = SECFailure;
  else if(SECITEM_CompareItem(&cert_issuer->derCert,
                              &issuer->derCert)!=SECEqual)
    res = SECFailure;

  CERT_DestroyCertificate(cert);
  CERT_DestroyCertificate(issuer);
  CERT_DestroyCertificate(cert_issuer);
  return res;
}
