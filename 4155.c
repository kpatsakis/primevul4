static int validate_safetynet_ca_root(json_t * j_params, gnutls_x509_crt_t cert_leaf, json_t * j_header_x5c) {
  gnutls_x509_crt_t cert_x509[(json_array_size(j_header_x5c)+1)], root_x509 = NULL;
  gnutls_x509_trust_list_t tlist = NULL;
  int ret = G_OK;
  unsigned int result, i;
  json_t * j_cert;
  unsigned char * header_cert_decoded;
  size_t header_cert_decoded_len;
  gnutls_datum_t cert_dat;

  cert_x509[0] = cert_leaf;
  for (i=1; i<json_array_size(j_header_x5c); i++) {
    j_cert = json_array_get(j_header_x5c, i);

    if ((header_cert_decoded = o_malloc(json_string_length(j_cert))) != NULL) {
      if (o_base64_decode((const unsigned char *)json_string_value(j_cert), json_string_length(j_cert), header_cert_decoded, &header_cert_decoded_len)) {
        if (!gnutls_x509_crt_init(&cert_x509[i])) {
          cert_dat.data = header_cert_decoded;
          cert_dat.size = header_cert_decoded_len;
          if ((ret = gnutls_x509_crt_import(cert_x509[i], &cert_dat, GNUTLS_X509_FMT_DER)) < 0) {
            y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error gnutls_x509_crt_import: %d", ret);
            ret = G_ERROR;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error gnutls_x509_crt_init");
          ret = G_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error o_base64_decode x5c leaf");
        ret = G_ERROR;
      }
      o_free(header_cert_decoded);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error allocating resources for header_cert_decoded");
      ret = G_ERROR_MEMORY;
    }
  }

  if (ret == G_OK) {
    cert_dat.data = (unsigned char *)json_string_value(json_object_get(json_object_get(j_params, "google-root-ca-r2-content"), "x509"));
    cert_dat.size = json_string_length(json_object_get(json_object_get(j_params, "google-root-ca-r2-content"), "x509"));
    if (!gnutls_x509_crt_init(&cert_x509[json_array_size(j_header_x5c)]) &&
        !gnutls_x509_crt_import(cert_x509[json_array_size(j_header_x5c)], &cert_dat, GNUTLS_X509_FMT_PEM)) {
      if (!gnutls_x509_crt_init(&root_x509) &&
          !gnutls_x509_crt_import(root_x509, &cert_dat, GNUTLS_X509_FMT_PEM)) {
        if (!gnutls_x509_trust_list_init(&tlist, 0)) {
          if (gnutls_x509_trust_list_add_cas(tlist, &root_x509, 1, 0) >= 0) {
            if (gnutls_x509_trust_list_verify_crt(tlist, cert_x509, (json_array_size(j_header_x5c)+1), 0, &result, NULL) >= 0) {
              if (!result) {
                ret = G_OK;
              } else {
                y_log_message(Y_LOG_LEVEL_DEBUG, "validate_safetynet_ca_root - certificate chain invalid");
                ret = G_ERROR;
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error gnutls_x509_trust_list_verify_crt");
              ret = G_ERROR;
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error gnutls_x509_trust_list_add_cas");
            ret = G_ERROR;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error gnutls_x509_trust_list_init");
          ret = G_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error import root cert");
        ret = G_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "validate_safetynet_ca_root - Error import last cert");
      ret = G_ERROR;
    }
  }
  // Clean after me
  for (i=1; i<json_array_size(j_header_x5c); i++) {
    gnutls_x509_crt_deinit(cert_x509[i]);
  }
  gnutls_x509_crt_deinit(cert_x509[json_array_size(j_header_x5c)]);
  gnutls_x509_trust_list_deinit(tlist, 1);
  return ret;
}