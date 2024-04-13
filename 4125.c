int user_auth_scheme_module_can_use(struct config_module * config, const char * username, void * cls) {
  json_t * j_user_id, * j_credential;
  int ret;

  j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 0);
  if (check_result_value(j_user_id, G_OK)) {
    j_credential = get_credential_list(config, (json_t *)cls, username, 1);
    if (check_result_value(j_credential, G_OK)) {
      ret = GLEWLWYD_IS_REGISTERED;
    } else if (check_result_value(j_credential, G_ERROR_NOT_FOUND)) {
      ret = GLEWLWYD_IS_AVAILABLE;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_can_use webauthn - Error get_credential_list");
      ret = GLEWLWYD_IS_NOT_AVAILABLE;
    }
    json_decref(j_credential);
  } else if (check_result_value(j_user_id, G_ERROR_NOT_FOUND)) {
    ret = GLEWLWYD_IS_AVAILABLE;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_can_use webauthn - Error get_user_id_from_username");
    ret = GLEWLWYD_IS_NOT_AVAILABLE;
  }
  json_decref(j_user_id);
  return ret;
}