json_t * user_auth_scheme_module_register(struct config_module * config, const struct _u_request * http_request, const char * username, json_t * j_scheme_data, void * cls) {
  UNUSED(config);
  UNUSED(http_request);
  json_t * j_return, * j_result, * j_credential, * j_user_id, * j_assertion;
  int res;

  if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "new-credential")) {
    j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 1);
    if (check_result_value(j_user_id, G_OK)) {
      j_credential = generate_new_credential(config, (json_t *)cls, username);
      if (check_result_value(j_credential, G_OK)) {
        j_return = json_pack("{sis{sOsOsOsss{sOss}sO}}",
                              "result", G_OK,
                              "response",
                                "session", json_object_get(json_object_get(j_credential, "credential"), "session"),
                                "challenge", json_object_get(json_object_get(j_credential, "credential"), "challenge"),
                                "pubKey-cred-params", json_object_get((json_t *)cls, "pubKey-cred-params"),
                                "attestation-required", json_object_get((json_t *)cls, "force-fmt-none")==json_true()?"none":"direct",
                                "user",
                                  "id", json_object_get(j_user_id, "user_id"),
                                  "name", username,
                                "rpId", json_object_get((json_t *)cls, "rp-origin")
                             );
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error generate_new_credential");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      json_decref(j_credential);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_user_id_from_username");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_user_id);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "register-credential")) {
    j_credential = get_credential_from_session(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "session")));
    if (check_result_value(j_credential, G_OK)) {
      j_result = register_new_attestation(config, (json_t *)cls, j_scheme_data, json_object_get(j_credential, "credential"));
      if (check_result_value(j_result, G_OK)) {
        j_return = json_pack("{si}", "result", G_OK);
      } else if (check_result_value(j_result, G_ERROR_UNAUTHORIZED)) {
        j_return = json_pack("{sisO}", "result", G_ERROR_UNAUTHORIZED, "response", json_object_get(j_result, "error"));
      } else if (check_result_value(j_result, G_ERROR_PARAM)) {
        j_return = json_pack("{sisO}", "result", G_ERROR_PARAM, "response", json_object_get(j_result, "error"));
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error register_new_attestation");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      json_decref(j_result);
    } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    } else if (check_result_value(j_credential, G_ERROR_PARAM)) {
      j_return = json_pack("{si}", "result", G_ERROR_PARAM);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential_from_session");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "remove-credential") && json_string_length(json_object_get(j_scheme_data, "credential_id"))) {
    j_credential = get_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")));
    if (check_result_value(j_credential, G_OK)) {
      if ((res = update_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")), 4)) == G_OK) {
        j_return = json_pack("{si}", "result", G_OK);
      } else if (res == G_ERROR_PARAM) {
        j_return = json_pack("{si}", "result", G_ERROR_PARAM);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error update_credential");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
    } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "disable-credential") && json_string_length(json_object_get(j_scheme_data, "credential_id"))) {
    j_credential = get_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")));
    if (check_result_value(j_credential, G_OK)) {
      if ((res = update_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")), 3)) == G_OK) {
        j_return = json_pack("{si}", "result", G_OK);
      } else if (res == G_ERROR_PARAM) {
        j_return = json_pack("{si}", "result", G_ERROR_PARAM);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error update_credential");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
    } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "enable-credential") && json_string_length(json_object_get(j_scheme_data, "credential_id"))) {
    j_credential = get_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")));
    if (check_result_value(j_credential, G_OK)) {
      if ((res = update_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")), 1)) == G_OK) {
        j_return = json_pack("{si}", "result", G_OK);
      } else if (res == G_ERROR_PARAM) {
        j_return = json_pack("{si}", "result", G_ERROR_PARAM);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error update_credential");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
    } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "edit-credential") && json_string_length(json_object_get(j_scheme_data, "credential_id")) && json_string_length(json_object_get(j_scheme_data, "name"))) {
    j_credential = get_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")));
    if (check_result_value(j_credential, G_OK)) {
      if ((res = update_credential_name(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "credential_id")), json_string_value(json_object_get(j_scheme_data, "name")))) == G_OK) {
        j_return = json_pack("{si}", "result", G_OK);
      } else if (res == G_ERROR_PARAM) {
        j_return = json_pack("{si}", "result", G_ERROR_PARAM);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error update_credential_name");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
    } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "trigger-assertion")) {
    j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 0);
    if (check_result_value(j_user_id, G_OK)) {
      j_credential = get_credential_list(config, (json_t *)cls, username, 1);
      if (check_result_value(j_credential, G_OK)) {
        j_assertion = generate_new_assertion(config, (json_t *)cls, username, 1);
        if (check_result_value(j_assertion, G_OK)) {
          j_return = json_pack("{sis{sOsOsOs{sOss}sO}}",
                              "result", G_OK,
                              "response",
                                "allowCredentials", json_object_get(j_credential, "credential"),
                                "session", json_object_get(json_object_get(j_assertion, "assertion"), "session"),
                                "challenge", json_object_get(json_object_get(j_assertion, "assertion"), "challenge"),
                                "user",
                                  "id", json_object_get(j_user_id, "user_id"),
                                  "name", username,
                                "rpId", json_object_get((json_t *)cls, "rp-origin")
                              );
        } else if (check_result_value(j_assertion, G_ERROR_UNAUTHORIZED)) {
          j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error register_new_assertion");
          j_return = json_pack("{si}", "result", G_ERROR);
        }
        json_decref(j_assertion);
      } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
        j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error get_credential_list");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      json_decref(j_credential);
    } else if (check_result_value(j_user_id, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_user_id_from_username");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_user_id);
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_scheme_data, "register")), "validate-assertion")) {
    j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 0);
    if (check_result_value(j_user_id, G_OK)) {
      j_assertion = get_assertion_from_session(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "session")), 1);
      if (check_result_value(j_assertion, G_OK)) {
        if ((res = check_assertion(config, (json_t *)cls, username, j_scheme_data, json_object_get(j_assertion, "assertion"))) == G_OK) {
          j_return = json_pack("{si}", "result", G_OK);
        } else if (res == G_ERROR_UNAUTHORIZED || res == G_ERROR_PARAM) {
          j_return = json_pack("{si}", "result", res);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error check_assertion");
          j_return = json_pack("{si}", "result", G_ERROR);
        }
      } else if (check_result_value(j_assertion, G_ERROR_NOT_FOUND)) {
        j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      json_decref(j_assertion);
    } else if (check_result_value(j_user_id, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_validate webauthn - Error get_user_id_from_username");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_user_id);
  } else {
    j_return = json_pack("{si}", "result", G_ERROR_PARAM);
  }

  return j_return;
}