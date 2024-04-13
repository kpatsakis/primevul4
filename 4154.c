static int validate_certificate_from_root(json_t * j_params, gnutls_x509_crt_t cert_leaf, cbor_item_t * x5c_array) {
  int ret = G_ERROR_NOT_FOUND, res;
  unsigned int result;
  gnutls_datum_t cert_dat = {NULL, 0}, issuer_dat = {NULL, 0};
  gnutls_x509_trust_list_t tlist = NULL;
  gnutls_x509_crt_t cert_x509[cbor_array_size(x5c_array)], root_x509 = NULL;
  json_t * j_cert = NULL;
  cbor_item_t * cbor_cert = NULL;
  size_t index = 0, i = 0, x5c_array_size = cbor_array_size(x5c_array);
  char * issuer;

  for (i=0; i<x5c_array_size+1; i++) {
    cert_x509[i] = NULL;
  }
  if ((res = gnutls_x509_crt_get_issuer_dn2(cert_leaf, &issuer_dat)) >= 0) {
    issuer = o_strndup((const char *)issuer_dat.data, issuer_dat.size);
    json_array_foreach(json_object_get(j_params, "root-ca-array"), index, j_cert) {
      if (0 == o_strcmp(issuer, json_string_value(json_object_get(j_cert, "dn")))) {
        cert_dat.data = (unsigned char *)json_string_value(json_object_get(j_cert, "x509"));
        cert_dat.size = json_string_length(json_object_get(j_cert, "x509"));
        if (!gnutls_x509_crt_init(&root_x509) && !gnutls_x509_crt_import(root_x509, &cert_dat, GNUTLS_X509_FMT_PEM)) {
          cert_x509[0] = cert_leaf;
          for (i=1; i<x5c_array_size; i++) {
            cbor_cert = cbor_array_get(x5c_array, i);
            cert_dat.data = cbor_bytestring_handle(cbor_cert);
            cert_dat.size = cbor_bytestring_length(cbor_cert);
            if (gnutls_x509_crt_init(&cert_x509[i]) < 0 || gnutls_x509_crt_import(cert_x509[i], &cert_dat, GNUTLS_X509_FMT_DER) < 0) {
              y_log_message(Y_LOG_LEVEL_ERROR, "validate_certificate_from_root - Error import chain cert at index %zu", i);
              ret = G_ERROR;
            }
            cbor_decref(&cbor_cert);
          }
          cert_x509[x5c_array_size] = root_x509;
          ret = G_OK;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "validate_certificate_from_root - Error import root cert");
          ret = G_ERROR;
        }
      }
    }
    o_free(issuer);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "validate_certificate_from_root - Error gnutls_x509_crt_get_issuer_dn2: %d", res);
    ret = G_ERROR;
  }
  gnutls_free(issuer_dat.data);

  if (ret == G_OK) {
    if (!gnutls_x509_trust_list_init(&tlist, 0)) {
      if (gnutls_x509_trust_list_add_cas(tlist, &root_x509, 1, 0) >= 0) {
        if (gnutls_x509_trust_list_verify_crt(tlist, cert_x509, x5c_array_size+1, 0, &result, NULL) >= 0) {
          if (result) {
            y_log_message(Y_LOG_LEVEL_DEBUG, "validate_certificate_from_root - certificate chain invalid");
            ret = G_ERROR;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "validate_certificate_from_root - Error gnutls_x509_trust_list_verify_crt");
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