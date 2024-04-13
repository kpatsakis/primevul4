static json_t * check_attestation_android_safetynet(json_t * j_params, cbor_item_t * auth_data, cbor_item_t * att_stmt, const unsigned char * client_data) {
  json_t * j_error = json_array(), * j_return;
  unsigned char pubkey_export[1024] = {0}, cert_export[32] = {0}, cert_export_b64[64], client_data_hash[32], * nonce_base = NULL, nonce_base_hash[32], * nonce_base_hash_b64 = NULL, * header_cert_decoded = NULL;
  char * message = NULL, * response_token = NULL, issued_to[128] = {0}, * jwt_header = NULL;
  size_t pubkey_export_len = 1024, cert_export_len = 32, cert_export_b64_len, issued_to_len = 128, client_data_hash_len = 32, nonce_base_hash_len = 32, nonce_base_hash_b64_len = 0, header_cert_decoded_len = 0;
  gnutls_pubkey_t pubkey = NULL;
  gnutls_x509_crt_t cert = NULL;
  cbor_item_t * key, * response = NULL;
  int i, ret;
  jwt_t * j_response = NULL;
  json_t * j_header_x5c = NULL, * j_cert = NULL, * j_header = NULL, * j_value = NULL;
  gnutls_datum_t cert_dat;
  int has_ver = 0;

  if (j_error != NULL) {
    do {
      // Step 1
      if (!cbor_isa_map(att_stmt) || cbor_map_size(att_stmt) != 2) {
        json_array_append_new(j_error, json_string("CBOR map value 'attStmt' invalid format"));
        break;
      }
      for (i=0; i<2; i++) {
        key = cbor_map_handle(att_stmt)[i].key;
        if (cbor_isa_string(key)) {
          if (0 == o_strncmp((const char *)cbor_string_handle(key), "ver", MIN(o_strlen("ver"), cbor_string_length(key))) && cbor_isa_string(cbor_map_handle(att_stmt)[i].value)) {
            has_ver = 1;
          } else if (0 == o_strncmp((const char *)cbor_string_handle(key), "response", MIN(o_strlen("response"), cbor_string_length(key))) && cbor_isa_bytestring(cbor_map_handle(att_stmt)[i].value)) {
            response = cbor_map_handle(att_stmt)[i].value;
          } else {
            message = msprintf("attStmt map element %d key is not valid: '%.*s'", i, cbor_string_length(key), cbor_string_handle(key));
            json_array_append_new(j_error, json_string(message));
            o_free(message);
            break;
          }
        } else {
          message = msprintf("attStmt map element %d key is not a string", i);
          json_array_append_new(j_error, json_string(message));
          o_free(message);
          break;
        }
      }

      if (!has_ver) {
        json_array_append_new(j_error, json_string("version invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error ver missing");
        break;
      }

      if (!generate_digest_raw(digest_SHA256, client_data, o_strlen((char *)client_data), client_data_hash, &client_data_hash_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error generate_digest_raw client_data");
        break;
      }

      if ((nonce_base = o_malloc(32 + cbor_bytestring_length(auth_data))) == NULL) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error allocating resources for nonce_base");
        break;
      }
      memcpy(nonce_base, cbor_bytestring_handle(auth_data), cbor_bytestring_length(auth_data));
      memcpy(nonce_base+cbor_bytestring_length(auth_data), client_data_hash, client_data_hash_len);

      if (!generate_digest_raw(digest_SHA256, nonce_base, 32 + cbor_bytestring_length(auth_data), nonce_base_hash, &nonce_base_hash_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error generate_digest_raw nonce_base");
        break;
      }

      if ((nonce_base_hash_b64 = o_malloc(64)) == NULL) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error allocating resources for nonce_base_hash_b64");
        break;
      }

      if (!o_base64_encode(nonce_base_hash, 32, nonce_base_hash_b64, &nonce_base_hash_b64_len)) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error o_base64_encode for nonce_base_hash_b64");
        break;
      }

      if (response == NULL) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error response missing");
        break;
      }

      if ((response_token = o_strndup((const char *)cbor_bytestring_handle(response), cbor_bytestring_length(response))) == NULL) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error o_strndup for response_token");
        break;
      }

      if (r_jwt_init(&j_response) != RHN_OK) {
        json_array_append_new(j_error, json_string("Internal error"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error r_jwt_init");
        break;
      }

      if (r_jwt_advanced_parse(j_response, response_token, R_PARSE_HEADER_X5C, 0) != RHN_OK) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error r_jwt_parse");
        break;
      }

      if (o_strcmp(r_jwt_get_claim_str_value(j_response, "nonce"), (const char *)nonce_base_hash_b64)) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error nonce invalid");
        break;
      }

      if (json_integer_value(json_object_get(j_params, "ctsProfileMatch")) != -1 && json_integer_value(json_object_get(j_params, "ctsProfileMatch")) != ((j_value = r_jwt_get_claim_json_t_value(j_response, "ctsProfileMatch"))==json_true()?1:0)) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error ctsProfileMatch invalid");
        json_decref(j_value);
        j_value = NULL;
        break;
      }
      json_decref(j_value);
      j_value = NULL;

      if (json_integer_value(json_object_get(j_params, "basicIntegrity")) != -1 && json_integer_value(json_object_get(j_params, "basicIntegrity")) != ((j_value = r_jwt_get_claim_json_t_value(j_response, "basicIntegrity"))==json_true()?1:0)) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error basicIntegrity invalid");
        j_value = NULL;
        break;
      }
      json_decref(j_value);
      j_value = NULL;

      if ((j_header_x5c = r_jwt_get_header_json_t_value(j_response, "x5c")) == NULL) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error parsing x5c JSON");
        break;
      }
      
      if (!json_is_string((j_cert = json_array_get(j_header_x5c, 0)))) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error x5c leaf not a string");
        break;
      }

      if ((header_cert_decoded = o_malloc(json_string_length(j_cert))) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error allocating resources for header_cert_decoded");
        break;
      }

      if (!o_base64_decode((const unsigned char *)json_string_value(j_cert), json_string_length(j_cert), header_cert_decoded, &header_cert_decoded_len)) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error o_base64_decode x5c leaf");
        break;
      }

      if (gnutls_x509_crt_init(&cert)) {
        json_array_append_new(j_error, json_string("internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error gnutls_x509_crt_init");
        break;
      }
      if (gnutls_pubkey_init(&pubkey)) {
        json_array_append_new(j_error, json_string("internal error"));
        y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error gnutls_pubkey_init");
        break;
      }
      cert_dat.data = header_cert_decoded;
      cert_dat.size = header_cert_decoded_len;
      if ((ret = gnutls_x509_crt_import(cert, &cert_dat, GNUTLS_X509_FMT_DER)) < 0) {
        json_array_append_new(j_error, json_string("Error importing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error gnutls_pcert_import_x509_raw: %d", ret);
        break;
      }
      
      if (r_jwt_verify_signature(j_response, NULL, 0) != RHN_OK) {
        json_array_append_new(j_error, json_string("Invalid signature"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error r_jwt_verify_signature");
        break;
      }

      if ((ret = gnutls_pubkey_import_x509(pubkey, cert, 0)) < 0) {
        json_array_append_new(j_error, json_string("Error importing x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error gnutls_pubkey_import_x509: %d", ret);
        break;
      }
      if ((ret = gnutls_x509_crt_get_key_id(cert, GNUTLS_KEYID_USE_SHA256, cert_export, &cert_export_len)) < 0) {
        json_array_append_new(j_error, json_string("Error exporting x509 certificate"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error gnutls_x509_crt_get_key_id: %d", ret);
        break;
      }
      if ((ret = gnutls_x509_crt_get_dn(cert, issued_to, &issued_to_len)) < 0) {
        json_array_append_new(j_error, json_string("Error x509 dn"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error gnutls_x509_crt_get_dn: %d", ret);
        break;
      }
      if (o_strnstr(issued_to, SAFETYNET_ISSUED_TO, issued_to_len) == NULL) {
        json_array_append_new(j_error, json_string("Error x509 dn"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - safetynet certificate issued for %.*s", issued_to_len, issued_to);
        break;
      }
      if (json_object_get(j_params, "google-root-ca-r2") != json_null()) {
        if ((ret = validate_safetynet_ca_root(j_params, cert, j_header_x5c)) == G_ERROR_UNAUTHORIZED) {
          json_array_append_new(j_error, json_string("Error x509 certificate chain validation"));
          break;
        } else if (ret != G_OK) {
          json_array_append_new(j_error, json_string("response invalid"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - safetynet certificate chain certificate validation error");
          break;
        }
      }
      if (!o_base64_encode(cert_export, cert_export_len, cert_export_b64, &cert_export_b64_len)) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error o_base64_encode cert_export");
        break;
      }
      if ((ret = gnutls_pubkey_export(pubkey, GNUTLS_X509_FMT_PEM, pubkey_export, &pubkey_export_len)) < 0) {
        json_array_append_new(j_error, json_string("response invalid"));
        y_log_message(Y_LOG_LEVEL_DEBUG, "check_attestation_android_safetynet - Error gnutls_pubkey_export: %d", ret);
        break;
      }

    } while (0);

    if (json_array_size(j_error)) {
      j_return = json_pack("{sisO}", "result", G_ERROR_PARAM, "error", j_error);
    } else {
      j_return = json_pack("{sis{ss%}}", "result", G_OK, "data", "certificate", cert_export_b64, cert_export_b64_len);
    }
    json_decref(j_error);
    json_decref(j_header);
    json_decref(j_header_x5c);
    gnutls_pubkey_deinit(pubkey);
    gnutls_x509_crt_deinit(cert);
    r_jwt_free(j_response);
    o_free(nonce_base);
    o_free(nonce_base_hash_b64);
    o_free(response_token);
    o_free(header_cert_decoded);
    o_free(jwt_header);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "check_attestation_android_safetynet - Error allocating resources for j_error");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  return j_return;
}