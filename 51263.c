static bool is_nss_error(CURLcode err)
{
  switch(err) {
  case CURLE_PEER_FAILED_VERIFICATION:
  case CURLE_SSL_CACERT:
  case CURLE_SSL_CERTPROBLEM:
  case CURLE_SSL_CONNECT_ERROR:
  case CURLE_SSL_ISSUER_ERROR:
    return true;

  default:
    return false;
  }
}
