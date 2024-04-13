json_t * user_auth_scheme_module_register_get(struct config_module * config, const struct _u_request * http_request, const char * username, void * cls) {
  UNUSED(http_request);
  json_t * j_return, * j_user_id, * j_credential_list;

  j_user_id = get_user_id_from_username(config, (json_t *)cls, username, 1);
  if (check_result_value(j_user_id, G_OK)) {
    j_credential_list = get_credential_list(config, (json_t *)cls, username, 0);
    if (check_result_value(j_credential_list, G_OK)) {
      j_return = json_pack("{sisO}", "result", G_OK, "response", json_object_get(j_credential_list, "credential"));
    } else if (check_result_value(j_credential_list, G_ERROR_NOT_FOUND)) {
      j_return = json_pack("{sis[]}", "result", G_OK, "response");
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register_get webauthn - Error get_credential_list");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential_list);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_register_get webauthn - Error get_user_id_from_username");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  json_decref(j_user_id);

  return j_return;
}