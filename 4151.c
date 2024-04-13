static json_t * check_attestation_packed(json_t * j_params, cbor_item_t * auth_data, cbor_item_t * att_stmt, const unsigned char * client_data, gnutls_pubkey_t g_key) {
  json_t * j_error = json_array(), * j_return;
  cbor_item_t * key, * alg = NULL, * sig = NULL, * x5c_array = NULL, * cert_leaf = NULL;
  size_t i, client_data_hash_len = 32, cert_export_len = 128, cert_export_b64_len = 0;
  char * message;
  gnutls_pubkey_t pubkey = NULL;
  gnutls_x509_crt_t cert = NULL;
  gnutls_datum_t cert_dat, data, signature, cert_issued_by;
  int ret, sig_alg = GNUTLS_SIGN_UNKNOWN;
  unsigned char client_data_hash[32], cert_export[128], cert_export_b64[256];

  data.data = NULL;
  UNUSED(j_params);

  if (j_error != NULL) {
    do {
      for (i=0; i<cbor_map_size(att_stmt); i++) {
        key = cbor_map_handle(att_stmt)[i].key;
        if (cbor_isa_string(key)) {
          if (0 == o_strncmp((const char *)cbor_string_handle(key), "alg", MIN(o_strlen("alg"), cbor_string_length(key))) && cbor_isa_negint(cbor_map_handle(att_stmt)[i].value)) {
            alg = cbor_map_handle(att_stmt)[i].value;
            if (cbor_get_int(alg) == 6) {
              sig_alg = GNUTLS_SIGN_ECDSA_SHA256;
            } else if (cbor_get_int(alg) == 34) {
              sig_alg = GNUTLS_SIGN_ECDSA_SHA384;
            } else if (cbor_get_int(alg) == 35) {
              sig_alg = GNUTLS_SIGN_ECDSA_SHA512;
            }
            if (sig_alg == GNUTLS_SIGN_UNKNOWN) {
              json_array_append_new(j_error, json_string("Signature algorithm not supported"));
              break;
            }
          } else if (0 == o_strncmp((const char *)cbor_string_handle(key), "sig", MIN(o_strlen("sig"), cbor_string_length(key))) && cbor_isa_bytestring(cbor_map_handle(att_stmt)[i].value)) {
            sig = cbor_map_handle(att_stmt)[i].value;
          } else if (0 == o_strncmp((const char *)cbor_string_handle(key), "x5c", MIN(o_strlen("x5c"), cbor_string_length(key))) && cbor_isa_array(cbor_map_handle(att_stmt)[i].value) && cbor_array_size(cbor_map_handle(att_stmt)[i].value)) {
            x5c_array = cbor_map_handle(att_stmt)[i].value;
          } else if (0 == o_strncmp((const char *)cbor_string_handle(key), "ecdaaKeyId", MIN(o_strlen("ecdaaKeyId"), cbor_string_length(key)))) {
            json_array_append_new(j_error, json_string("ecdaaKeyId not supported"));
            break;
          }
        } else {
          message = msprintf("attStmt map element %zu key is not a string", i);
          json_array_append_new(j_error, json_string(message));
          o_free(message);
          break;
        }
      }

      if (json_array_size(j_error)) {
        break;
      }

      if (alg == NULL || sig == NULL) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_packed - Error alg or sig are not mapped in att_stmt");
        break;
      }

      if (!generate_digest_raw(digest_SHA256, client_data, o_strlen((char *)client_data), client_data_hash, &client_data_hash_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error generate_digest_raw client_data");
        break;
      }

      if ((data.data = o_malloc(cbor_bytestring_length(auth_data) + client_data_hash_len)) == NULL) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error o_malloc data.data");
        break;
      }

      signature.data = cbor_bytestring_handle(sig);
      signature.size = cbor_bytestring_length(sig);

      memcpy(data.data, cbor_bytestring_handle(auth_data), cbor_bytestring_length(auth_data));
      memcpy(data.data + cbor_bytestring_length(auth_data), client_data_hash, client_data_hash_len);
      data.size = cbor_bytestring_length(auth_data) + client_data_hash_len;

      // packed disable SELF attestation for now
      if (x5c_array == NULL) {
        if (gnutls_pubkey_verify_data2(g_key, sig_alg, 0, &data, &signature)) {
          json_array_append_new(j_error, json_string("Invalid signature"));
          break;
        }

        cert_export_b64_len = 0;
        cert_export_b64[0] = '\0';
      } else {
        if (gnutls_x509_crt_init(&cert)) {
          json_array_append_new(j_error, json_string("check_attestation_packed - Error gnutls_x509_crt_init"));
          break;
        }
        if (gnutls_pubkey_init(&pubkey)) {
          json_array_append_new(j_error, json_string("check_attestation_packed - Error gnutls_pubkey_init"));
          break;
        }

        cert_leaf = cbor_array_get(x5c_array, 0);
        cert_dat.data = cbor_bytestring_handle(cert_leaf);
        cert_dat.size = cbor_bytestring_length(cert_leaf);

        if ((ret = gnutls_x509_crt_import(cert, &cert_dat, GNUTLS_X509_FMT_DER)) < 0) {
          json_array_append_new(j_error, json_string("Error importing x509 certificate"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error gnutls_pcert_import_x509_raw: %d", ret);
          break;
        }

        if ((ret = gnutls_pubkey_import_x509(pubkey, cert, 0)) < 0) {
          json_array_append_new(j_error, json_string("Error importing x509 certificate"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error gnutls_pubkey_import_x509: %d", ret);
          break;
        }

        if (gnutls_pubkey_verify_data2(pubkey, sig_alg, 0, &data, &signature)) {
          json_array_append_new(j_error, json_string("Invalid signature"));
          break;
        }

        if (validate_packed_leaf_certificate(cert, (cbor_bytestring_handle(auth_data)+ATTESTED_CRED_DATA_OFFSET)) != G_OK) {
          json_array_append_new(j_error, json_string("Invalid certificate"));
          break;
        }

        if ((ret = gnutls_x509_crt_get_key_id(cert, GNUTLS_KEYID_USE_SHA256, cert_export, &cert_export_len)) < 0) {
          json_array_append_new(j_error, json_string("Error exporting x509 certificate"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error gnutls_x509_crt_get_key_id: %d", ret);
          break;
        }

        if (json_object_get(j_params, "root-ca-list") != json_null() && validate_certificate_from_root(j_params, cert, x5c_array) != G_OK) {
          json_array_append_new(j_error, json_string("Unrecognized certificate authority"));
          if (gnutls_x509_crt_get_issuer_dn2(cert, &cert_issued_by) >= 0) {
            message = msprintf("Unrecognized certificate autohority: %.*s", cert_issued_by.size, cert_issued_by.data);
            y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - %s", message);
            o_free(message);
            gnutls_free(cert_issued_by.data);
          } else {
            y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Unrecognized certificate autohority (unable to get issuer dn)");
          }
          break;
        }

        if (!o_base64_encode(cert_export, cert_export_len, cert_export_b64, &cert_export_b64_len)) {
          json_array_append_new(j_error, json_string("Internal error"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_packed - Error o_base64_encode cert_export");
          break;
        }
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
    o_free(data.data);
    if (cert_leaf != NULL) {
      cbor_decref(&cert_leaf);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_packed - Error allocating resources for j_error");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  return j_return;
}