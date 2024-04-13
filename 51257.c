static CURLcode cert_stuff(struct connectdata *conn, int sockindex,
                           char *cert_file, char *key_file)
{
  struct Curl_easy *data = conn->data;
  CURLcode result;

  if(cert_file) {
    result = nss_load_cert(&conn->ssl[sockindex], cert_file, PR_FALSE);
    if(result) {
      const PRErrorCode err = PR_GetError();
      if(!display_error(conn, err, cert_file)) {
        const char *err_name = nss_error_to_name(err);
        failf(data, "unable to load client cert: %d (%s)", err, err_name);
      }

      return result;
    }
  }

  if(key_file || (is_file(cert_file))) {
    if(key_file)
      result = nss_load_key(conn, sockindex, key_file);
    else
      /* In case the cert file also has the key */
      result = nss_load_key(conn, sockindex, cert_file);
    if(result) {
      const PRErrorCode err = PR_GetError();
      if(!display_error(conn, err, key_file)) {
        const char *err_name = nss_error_to_name(err);
        failf(data, "unable to load client key: %d (%s)", err, err_name);
      }

      return result;
    }
  }

  return CURLE_OK;
}
