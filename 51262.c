static bool is_cc_error(PRInt32 err)
{
  switch(err) {
  case SSL_ERROR_BAD_CERT_ALERT:
  case SSL_ERROR_EXPIRED_CERT_ALERT:
  case SSL_ERROR_REVOKED_CERT_ALERT:
    return true;

  default:
    return false;
  }
}
