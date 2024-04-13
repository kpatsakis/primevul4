int user_auth_scheme_module_validate(struct config_module * config, const struct _u_request * http_request, const char * username, json_t * j_scheme_data, void * cls) {
  UNUSED(http_request);
  int ret, res;
  json_t * j_user_id, * j_assertion;

  j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 0);
  if (check_result_value(j_user_id, G_OK)) {
    j_assertion = get_assertion_from_session(config, (json_t *)cls, username, json_string_value(json_object_get(j_scheme_data, "session")), 0);
    if (check_result_value(j_assertion, G_OK)) {
      if ((res = check_assertion(config, (json_t *)cls, username, j_scheme_data, json_object_get(j_assertion, "assertion"))) == G_OK) {
        ret = G_OK;
      } else if (res == G_ERROR_UNAUTHORIZED) {
        ret = G_ERROR_UNAUTHORIZED;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_validate webauthn - Error check_assertion");
        ret = G_ERROR;
      }
    } else if (check_result_value(j_assertion, G_ERROR_NOT_FOUND)) {
      ret = G_ERROR_UNAUTHORIZED;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register webauthn - Error get_credential");
      ret = G_ERROR;
    }
    json_decref(j_assertion);
  } else if (check_result_value(j_user_id, G_ERROR_NOT_FOUND)) {
    ret = G_ERROR_UNAUTHORIZED;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_validate webauthn - Error get_user_id_from_username");
    ret = G_ERROR;
  }
  json_decref(j_user_id);

  return ret;
}