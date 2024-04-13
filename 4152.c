json_t * user_auth_scheme_module_trigger(struct config_module * config, const struct _u_request * http_request, const char * username, json_t * j_scheme_trigger, void * cls) {
  UNUSED(j_scheme_trigger);
  json_t * j_return = NULL, * j_session = config->glewlwyd_module_callback_check_user_session(config, http_request, username), * j_credential, * j_assertion, * j_user_id, * j_credential_fake;
  unsigned char user_id_fake[64];

  if (check_result_value(j_session, G_OK) || json_object_get((json_t *)cls, "session-mandatory") == json_false()) {
    j_credential_fake = generate_credential_fake_list((json_t *)cls, username);
    if (check_result_value(j_credential_fake, G_OK)) {
      j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 0);
      if (check_result_value(j_user_id, G_OK)) {
        j_credential = get_credential_list(config, (json_t *)cls, username, 1);
        if (check_result_value(j_credential, G_OK)) {
          j_assertion = generate_new_assertion(config, (json_t *)cls, username, 0);
          if (check_result_value(j_assertion, G_OK)) {
            j_return = json_pack("{sis{sOsOsOs{sOss}sOsssi}}",
                                "result", G_OK,
                                "response",
                                  "allowCredentials", json_object_get(j_credential, "credential"),
                                  "session", json_object_get(json_object_get(j_assertion, "assertion"), "session"),
                                  "challenge", json_object_get(json_object_get(j_assertion, "assertion"), "challenge"),
                                  "user",
                                    "id", json_object_get(j_user_id, "user_id"),
                                    "name", username,
                                  "rpId", json_object_get((json_t *)cls, "rp-origin"),
                                  "attestation-required", json_object_get((json_t *)cls, "force-fmt-none")==json_true()?"none":"direct",
                                  "timeout", 60000
                                );
            if (json_object_get((json_t *)cls, "session-mandatory") == json_false()) {
              json_array_extend(json_object_get(json_object_get(j_return, "response"), "allowCredentials"), json_object_get(j_credential_fake, "credential"));
            }
          } else if (check_result_value(j_assertion, G_ERROR_UNAUTHORIZED)) {
            j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error register_new_assertion");
            j_return = json_pack("{si}", "result", G_ERROR);
          }
          json_decref(j_assertion);
        } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
          if (json_object_get((json_t *)cls, "session-mandatory") == json_false()) {
            j_assertion = generate_new_assertion(config, (json_t *)cls, username, 2);
            if (check_result_value(j_assertion, G_OK)) {
              j_return = json_pack("{sis{sOsOsOs{sOss}sO}}",
                                  "result", G_OK,
                                  "response",
                                    "allowCredentials", json_object_get(j_credential_fake, "credential"),
                                    "session", json_object_get(json_object_get(j_assertion, "assertion"), "session"),
                                    "challenge", json_object_get(json_object_get(j_assertion, "assertion"), "challenge"),
                                    "user",
                                      "id", json_object_get(j_user_id, "user_id"),
                                      "name", username,
                                    "rpId", json_object_get((json_t *)cls, "rp-origin")
                                  );
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error register_new_assertion");
              j_return = json_pack("{si}", "result", G_ERROR);
            }
            json_decref(j_assertion);
          } else {
            j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error get_credential_list");
          j_return = json_pack("{si}", "result", G_ERROR);
        }
        json_decref(j_credential);
      } else if (check_result_value(j_user_id, G_ERROR_NOT_FOUND)) {
        if (json_object_get((json_t *)cls, "session-mandatory") == json_false()) {
          if (generate_fake_user_id((json_t *)cls, username, user_id_fake) == G_OK) {
            j_assertion = generate_new_assertion(config, (json_t *)cls, username, 2);
            if (check_result_value(j_assertion, G_OK)) {
              j_return = json_pack("{sis{sOsOsOs{ssss}sO}}",
                                  "result", G_OK,
                                  "response",
                                    "allowCredentials", json_object_get(j_credential_fake, "credential"),
                                    "session", json_object_get(json_object_get(j_assertion, "assertion"), "session"),
                                    "challenge", json_object_get(json_object_get(j_assertion, "assertion"), "challenge"),
                                    "user",
                                      "id", user_id_fake,
                                      "name", username,
                                    "rpId", json_object_get((json_t *)cls, "rp-origin")
                                  );
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error register_new_assertion");
              j_return = json_pack("{si}", "result", G_ERROR);
            }
            json_decref(j_assertion);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error generate_fake_user_id");
            j_return = json_pack("{si}", "result", G_ERROR);
          }
        } else {
          j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_user_id_from_username");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      json_decref(j_user_id);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error generate_credential_fake");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential_fake);
  } else if (check_result_value(j_session, G_ERROR_UNAUTHORIZED)) {
    j_return = json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_trigger webauthn - Error glewlwyd_module_callback_check_user_session");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  json_decref(j_session);
  return j_return;
}