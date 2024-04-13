static int update_credential(struct config_module * config, json_t * j_params, const char * username, const char * credential_id, int status) {
  json_t * j_query;
  char * username_escaped, * mod_name_escaped, * username_clause;
  int res, ret;

  username_escaped = h_escape_string_with_quotes(config->conn, username);
  mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
  username_clause = msprintf(" = (SELECT gswu_id FROM "G_TABLE_WEBAUTHN_USER" WHERE UPPER(gswu_username) = UPPER(%s) AND gswu_mod_name = %s)", username_escaped, mod_name_escaped);
  j_query = json_pack("{sss{si}s{sss{ssss}}}",
                      "table",
                      G_TABLE_WEBAUTHN_CREDENTIAL,
                      "set",
                        "gswc_status",
                        status,
                      "where",
                        "gswc_credential_id",
                        credential_id,
                        "gswu_id",
                          "operator",
                          "raw",
                          "value",
                          username_clause);
  o_free(username_clause);
  o_free(username_escaped);
  o_free(mod_name_escaped);
  res = h_update(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    ret = G_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_credential - Error executing j_query");
    config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
    ret = G_ERROR_DB;
  }
  return ret;
}