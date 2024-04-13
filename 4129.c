static json_t * is_scheme_parameters_valid(json_t * j_params) {
  json_t * j_return, * j_error, * j_element = NULL, * j_cert;
  size_t index = 0;
  json_int_t pubkey;
  char * message;

  if (json_is_object(j_params)) {
    j_error = json_array();
    if (j_error != NULL) {
      if (!json_is_boolean(json_object_get(j_params, "session-mandatory"))) {
        json_array_append_new(j_error, json_string("session-mandatory is mandatory and must be a boolean"));
      }
      if (json_object_get(j_params, "seed") != NULL && !json_is_string(json_object_get(j_params, "seed"))) {
        json_array_append_new(j_error, json_string("seed is optional and must be a string"));
      }
      if (json_integer_value(json_object_get(j_params, "challenge-length")) <= 0) {
        json_array_append_new(j_error, json_string("challenge-length is mandatory and must be a positive integer"));
      }
      if (json_integer_value(json_object_get(j_params, "credential-expiration")) <= 0) {
        json_array_append_new(j_error, json_string("credential-expiration is mandatory and must be a positive integer"));
      }
      if (json_integer_value(json_object_get(j_params, "credential-assertion")) <= 0) {
        json_array_append_new(j_error, json_string("credential-assertion is mandatory and must be a positive integer"));
      }
      if (!json_string_length(json_object_get(j_params, "rp-origin"))) {
        json_array_append_new(j_error, json_string("rp-origin is mandatory and must be a non empty string"));
      }
      if (!json_array_size(json_object_get(j_params, "pubKey-cred-params"))) {
        json_array_append_new(j_error, json_string("pubKey-cred-params is mandatory and must be a non empty JSON array"));
      } else {
        json_array_foreach(json_object_get(j_params, "pubKey-cred-params"), index, j_element) {
          pubkey = json_integer_value(j_element);
          //if (pubkey != -7 && pubkey != -35 && pubkey != -36 && pubkey != -257 && pubkey != -258 && pubkey != -259) {
          if (pubkey != ECDSA256 && pubkey != ECDSA384 && pubkey != ECDSA512) {
            //json_array_append_new(j_error, json_string("pubKey-cred-params elements values available are -7, -35, -36 (ECDSA) or -257, -258, -259 (RSA)"));
            json_array_append_new(j_error, json_string("pubKey-cred-params elements values available are -7, -35, -36 (ECDSA)"));
          }
        }
      }
      if (json_object_get(j_params, "ctsProfileMatch") != NULL && (!json_is_integer(json_object_get(j_params, "ctsProfileMatch")) || json_integer_value(json_object_get(j_params, "ctsProfileMatch")) < -1 || json_integer_value(json_object_get(j_params, "ctsProfileMatch")) > 1)) {
        json_array_append_new(j_error, json_string("ctsProfileMatch is optional and must be an integer between -1 and 1"));
      }
      if (json_object_get(j_params, "basicIntegrity") != NULL && (!json_is_integer(json_object_get(j_params, "basicIntegrity")) || json_integer_value(json_object_get(j_params, "basicIntegrity")) < -1 || json_integer_value(json_object_get(j_params, "basicIntegrity")) > 1)) {
        json_array_append_new(j_error, json_string("basicIntegrity is optional and must be an integer between -1 and 1"));
      }
      if (json_object_get(j_params, "google-root-ca-r2") != NULL && !json_is_string(json_object_get(j_params, "google-root-ca-r2"))) {
        json_array_append_new(j_error, json_string("google-root-ca-r2 is optional and must be a string"));
      } else if (json_string_length(json_object_get(j_params, "google-root-ca-r2"))) {
        j_cert = get_cert_from_file_path(json_string_value(json_object_get(j_params, "google-root-ca-r2")));
        if (check_result_value(j_cert, G_OK)) {
          json_object_set(j_params, "google-root-ca-r2-content", json_object_get(j_cert, "certificate"));
        } else {
          message = msprintf("Error parsing google-root-ca-r2 certificate file %s", json_string_value(json_object_get(j_params, "google-root-ca-r2")));
          json_array_append_new(j_error, json_string(message));
          o_free(message);
        }
        json_decref(j_cert);
      }
      if (json_object_get(j_params, "apple-root-ca") != NULL && !json_is_string(json_object_get(j_params, "apple-root-ca"))) {
        json_array_append_new(j_error, json_string("apple-root-ca is optional and must be a string"));
      } else if (json_string_length(json_object_get(j_params, "apple-root-ca"))) {
        j_cert = get_cert_from_file_path(json_string_value(json_object_get(j_params, "apple-root-ca")));
        if (check_result_value(j_cert, G_OK)) {
          json_object_set(j_params, "apple-root-ca-content", json_object_get(j_cert, "certificate"));
        } else {
          message = msprintf("Error parsing apple-root-ca certificate file %s", json_string_value(json_object_get(j_params, "apple-root-ca")));
          json_array_append_new(j_error, json_string(message));
          o_free(message);
        }
        json_decref(j_cert);
      }
      if (json_object_get(j_params, "root-ca-list") != NULL) {
        if (!json_is_array(json_object_get(j_params, "root-ca-list"))) {
          json_array_append_new(j_error, json_string("root-ca-list is optional and must be an array of strings"));
        } else {
          json_object_set_new(j_params, "root-ca-array", json_array());
          json_array_foreach(json_object_get(j_params, "root-ca-list"), index, j_element) {
            if (!json_string_length(j_element)) {
              json_array_append_new(j_error, json_string("root-ca-list is optional and must be an array of strings"));
            } else {
              j_cert = get_cert_from_file_path(json_string_value(j_element));
              if (check_result_value(j_cert, G_OK)) {
                json_array_append(json_object_get(j_params, "root-ca-array"), json_object_get(j_cert, "certificate"));
              } else {
                message = msprintf("Error parsing certificate file %s", json_string_value(j_element));
                json_array_append_new(j_error, json_string(message));
                o_free(message);
              }
              json_decref(j_cert);
            }
          }
        }
      }
      if (json_object_get(j_params, "force-fmt-none") != NULL && !json_is_boolean(json_object_get(j_params, "force-fmt-none"))) {
        json_array_append_new(j_error, json_string("allow-fmt-none is optional and must be a boolean"));
      }
      if (json_object_get(j_params, "fmt") != NULL && (!json_is_object(json_object_get(j_params, "fmt")) || (json_object_get(json_object_get(j_params, "fmt"), "packed") != json_true() && json_object_get(json_object_get(j_params, "fmt"), "tpm") != json_true() && json_object_get(json_object_get(j_params, "fmt"), "android-key") != json_true() && json_object_get(json_object_get(j_params, "fmt"), "android-safetynet") != json_true() && json_object_get(json_object_get(j_params, "fmt"), "fido-u2f") != json_true() && json_object_get(json_object_get(j_params, "fmt"), "none") != json_true()))) {
        json_array_append_new(j_error, json_string("fmt must be a JSON object filled with supported formats: 'packed' 'tpm', 'android-key', 'android-safetynet', 'fido-u2f', 'none'"));
      }
      if (json_array_size(j_error)) {
        j_return = json_pack("{sisO}", "result", G_ERROR_PARAM, "error", j_error);
      } else {
        j_return = json_pack("{si}", "result", G_OK);
      }
      json_decref(j_error);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "is_scheme_parameters_valid - Error allocating resources for j_error");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
  } else {
    j_return = json_pack("{sis[s]}", "result", G_ERROR_PARAM, "error", "parameters must be a JSON object");
  }
  return j_return;
}