int user_auth_scheme_module_deregister(struct config_module * config, const char * username, void * cls) {
  json_t * j_user_id, * j_credential_list, * j_credential, * j_element = NULL;
  size_t index = 0;
  int ret;

  j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 1);
  if (check_result_value(j_user_id, G_OK)) {
    j_credential_list = get_credential_list(config, (json_t *)cls, username, 0);
    if (check_result_value(j_credential_list, G_OK)) {
      json_array_foreach(json_object_get(j_credential_list, "credential"), index, j_element) {
        j_credential = get_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_element, "credential_id")));
        if (check_result_value(j_credential, G_OK)) {
          if (update_credential(config, (json_t *)cls, username, json_string_value(json_object_get(j_element, "credential_id")), 4) != G_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_deregister webauthn - Error update_credential");
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_deregister webauthn - Error get_credential");
        }
        json_decref(j_credential);
      }
      ret = G_OK;
    } else if (check_result_value(j_credential_list, G_ERROR_NOT_FOUND)) {
      ret = G_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_deregister webauthn - Error get_credential_list");
      ret = G_ERROR;
    }
    json_decref(j_credential_list);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_deregister webauthn - Error get_user_id_from_username");
    ret = G_ERROR;
  }
  json_decref(j_user_id);

  return ret;
}