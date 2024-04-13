static json_t * check_attestation_apple(json_t * j_params, cbor_item_t * auth_data, cbor_item_t * att_stmt, const unsigned char * client_data, gnutls_pubkey_t g_key) {
  json_t * j_error = json_array(), * j_return;
  cbor_item_t * key, * x5c_array = NULL, * cert_leaf = NULL;
  size_t client_data_hash_len = 32, nonce_base_len = 0, expected_nonce_len = 32, cert_nonce_len = 64, cert_export_len = 128, cert_export_b64_len = 0;
  char * message;
  gnutls_pubkey_t pubkey = NULL;
  gnutls_x509_crt_t cert = NULL;
  gnutls_datum_t cert_dat;
  int ret;
  unsigned char client_data_hash[32], * nonce_base = NULL, expected_nonce[32], cert_nonce[64], cert_export[128], cert_export_b64[256];
  const unsigned char apple_aaguid[] = {0xf2, 0x4a, 0x8e, 0x70, 0xd0, 0xd3, 0xf8, 0x2c, 0x29, 0x37, 0x32, 0x52, 0x3c, 0xc4, 0xde, 0x5a};
  unsigned int bits = 0;
  gnutls_datum_t x, y, g_x, g_y;
  gnutls_ecc_curve_t curve, g_curve;

  if (j_error != NULL) {
    do {
      if (cbor_map_size(att_stmt) != 1) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_apple - Error attStmt has not only one element");
        break;
      }

      key = cbor_map_handle(att_stmt)[0].key;
      if (cbor_isa_string(key)) {
        if (0 == o_strncmp((const char *)cbor_string_handle(key), "x5c", MIN(o_strlen("x5c"), cbor_string_length(key))) && cbor_isa_array(cbor_map_handle(att_stmt)[0].value) && cbor_array_size(cbor_map_handle(att_stmt)[0].value)) {
          x5c_array = cbor_map_handle(att_stmt)[0].value;
        }
      } else {
        message = msprintf("attStmt map element 0 key is not a string");
        json_array_append_new(j_error, json_string(message));
        o_free(message);
        break;
      }

      if (!generate_digest_raw(digest_SHA256, client_data, o_strlen((char *)client_data), client_data_hash, &client_data_hash_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error generate_digest_raw client_data");
        break;
      }

      nonce_base_len = cbor_bytestring_length(auth_data) + client_data_hash_len;
      if ((nonce_base = o_malloc(nonce_base_len)) == NULL) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error o_malloc nonce_base");
        break;
      }
      memcpy(nonce_base, cbor_bytestring_handle(auth_data), cbor_bytestring_length(auth_data));
      memcpy(nonce_base+cbor_bytestring_length(auth_data), client_data_hash, client_data_hash_len);

      if (!generate_digest_raw(digest_SHA256, nonce_base, nonce_base_len, expected_nonce, &expected_nonce_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error generate_digest_raw expected_nonce");
        break;
      }

      if (x5c_array == NULL) {
        json_array_append_new(j_error, json_string("Invalid x5c"));
        break;
      }

      if (gnutls_x509_crt_init(&cert)) {
        json_array_append_new(j_error, json_string("check_attestation_apple - Error gnutls_x509_crt_init"));
        break;
      }

      cert_leaf = cbor_array_get(x5c_array, 0);
      cert_dat.data = cbor_bytestring_handle(cert_leaf);
      cert_dat.size = cbor_bytestring_length(cert_leaf);

      if ((ret = gnutls_x509_crt_import(cert, &cert_dat, GNUTLS_X509_FMT_DER)) < 0) {
        json_array_append_new(j_error, json_string("Error importing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error gnutls_x509_crt_import: %d", ret);
        break;
      }
      
      if (gnutls_pubkey_init(&pubkey)) {
        json_array_append_new(j_error, json_string("check_attestation_apple - Error gnutls_pubkey_init"));
        break;
      }

      if ((ret = gnutls_pubkey_import_x509(pubkey, cert, 0)) < 0) {
        json_array_append_new(j_error, json_string("Error importing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error gnutls_pubkey_import_x509: %d", ret);
        break;
      }

      if (gnutls_pubkey_get_pk_algorithm(pubkey, &bits) != GNUTLS_PK_ECDSA) {
        json_array_append_new(j_error, json_string("Error parsing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error invalid public key type, not ECDSA");
        break;
      }

      if (gnutls_pubkey_export_ecc_raw2(pubkey, &curve, &x, &y, GNUTLS_EXPORT_FLAG_NO_LZ) < 0) {
        json_array_append_new(j_error, json_string("Error parsing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error gnutls_pubkey_export_ecc_raw2 pubkey");
        break;
      }

      if (gnutls_pubkey_export_ecc_raw2(g_key, &g_curve, &g_x, &g_y, GNUTLS_EXPORT_FLAG_NO_LZ) < 0) {
        json_array_append_new(j_error, json_string("Error parsing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error gnutls_pubkey_export_ecc_raw2 ");
        break;
      }

      if (x.size != g_x.size || memcmp(x.data, g_x.data, x.size)) {
        json_array_append_new(j_error, json_string("Error invalid x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error match cert x value ");
        break;
      }

      if (y.size != g_y.size || memcmp(y.data, g_y.data, y.size)) {
        json_array_append_new(j_error, json_string("Error invalid x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error match cert y value ");
        break;
      }

      if (gnutls_x509_crt_get_extension_by_oid(cert, G_APPLE_ANONYMOUS_ATTESTATION_OID, 0, cert_nonce, &cert_nonce_len, NULL) >= 0) {
        if (cert_nonce_len != expected_nonce_len+6) {
          json_array_append_new(j_error, json_string("Error invalid x509 certificate"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Invalid cert_nonce_len %zu", cert_nonce_len);
          break;
        }

        if (memcmp(expected_nonce, cert_nonce+6, expected_nonce_len)) {
          json_array_append_new(j_error, json_string("Error invalid x509 certificate"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Invalid cert_nonce match");
          break;
        }
      } else {
        json_array_append_new(j_error, json_string("Error invalid x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Error getting AppleAnonymousAttestation OID 1.2.840.113635.100.8.2 extension");
        break;
      }

      if (memcmp((cbor_bytestring_handle(auth_data)+ATTESTED_CRED_DATA_OFFSET), apple_aaguid, AAGUID_LEN)) {
        json_array_append_new(j_error, json_string("Error invalid certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_apple - Invalid aaguid match");
        break;
      }

      if (json_object_get(j_params, "apple-root-ca-content") != json_null()) {
        if (validate_apple_certificate_chain(j_params, cert, x5c_array) != G_OK) {
          json_array_append_new(j_error, json_string("Invalid certificate chain"));
          break;
        }
      }

      if ((ret = gnutls_x509_crt_get_key_id(cert, GNUTLS_KEYID_USE_SHA256, cert_export, &cert_export_len)) < 0) {
        json_array_append_new(j_error, json_string("Error exporting x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error gnutls_x509_crt_get_key_id: %d", ret);
        break;
      }

      if (!o_base64_encode(cert_export, cert_export_len, cert_export_b64, &cert_export_b64_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error o_base64_encode cert_export");
        break;
      }

    } while (0);

    if (json_array_size(j_error)) {
      j_return = json_pack("{sisO}", "result", G_ERROR_PARAM, "error", j_error);
    } else {
      j_return = json_pack("{sis{ss%}}", "result", G_OK, "data", "certificate", cert_export_b64, cert_export_b64_len);
    }
    json_decref(j_error);
    gnutls_x509_crt_deinit(cert);
    gnutls_pubkey_deinit(pubkey);
    gnutls_free(x.data);
    gnutls_free(y.data);
    gnutls_free(g_x.data);
    gnutls_free(g_y.data);
    o_free(nonce_base);
    if (cert_leaf != NULL) {
      cbor_decref(&cert_leaf);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_packed - Error allocating resources for j_error");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  return j_return;
}