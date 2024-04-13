static int validate_apple_certificate_chain(json_t * j_params, gnutls_x509_crt_t cert_leaf, cbor_item_t * x5c_array) {
  int ret = G_OK;
  size_t i, x5c_array_size = cbor_array_size(x5c_array);
  gnutls_x509_crt_t cert_x509[x5c_array_size], root_x509 = NULL;
  gnutls_x509_trust_list_t tlist = NULL;
  gnutls_datum_t cert_dat;
  cbor_item_t * cbor_cert = NULL;
  unsigned int result;

  cert_x509[0] = cert_leaf;
  for (i=1; i<x5c_array_size; i++) {
    cbor_cert = cbor_array_get(x5c_array, i);
    cert_dat.data = cbor_bytestring_handle(cbor_cert);
    cert_dat.size = cbor_bytestring_length(cbor_cert);
    if (gnutls_x509_crt_init(&cert_x509[i]) < 0 || gnutls_x509_crt_import(cert_x509[i], &cert_dat, GNUTLS_X509_FMT_DER) < 0) {
      y_log_message(Y_LOG_LEVEL_ERROR, "validate_apple_certificate_chain - Error import chain cert at index %zu", i);
      ret = G_ERROR;
    }
    cbor_decref(&cbor_cert);
  }

  if (ret == G_OK) {
    cert_dat.data = (unsigned char *)json_string_value(json_object_get(json_object_get(j_params, "apple-root-ca-content"), "x509"));
    cert_dat.size = json_string_length(json_object_get(json_object_get(j_params, "apple-root-ca-content"), "x509"));
    if (gnutls_x509_crt_init(&root_x509) || gnutls_x509_crt_import(root_x509, &cert_dat, GNUTLS_X509_FMT_PEM)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "validate_apple_certificate_chain - Error import root cert");
      ret = G_ERROR;
    }
  }

  if (ret == G_OK) {
    if (!gnutls_x509_trust_list_init(&tlist, 0)) {
      if (gnutls_x509_trust_list_add_cas(tlist, &root_x509, 1, 0) >= 0) {
        if (gnutls_x509_trust_list_verify_crt(tlist, cert_x509, x5c_array_size, 0, &result, NULL) >= 0) {
          if (result) {
            y_log_message(Y_LOG_LEVEL_DEBUG, "validate_apple_certificate_chain - certificate chain invalid");
            ret = G_ERROR_UNAUTHORIZED;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "validate_apple_certificate_chain - Error gnutls_x509_trust_list_verify_crt");
          ret = G_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "validate_certificate_from_root - Error gnutls_x509_trust_list_add_cas");
        ret = G_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "validate_certificate_from_root - Error gnutls_x509_trust_list_init");
      ret = G_ERROR;
    }
  }
  gnutls_x509_crt_deinit(root_x509);
  for (i=1; i<x5c_array_size; i++) {
    gnutls_x509_crt_deinit(cert_x509[i]);
  }
  gnutls_x509_trust_list_deinit(tlist, 0);
  return ret;
}