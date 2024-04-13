static json_t * get_credential(struct config_module * config, json_t * j_params, const char * username, const char * credential_id) {
  json_t * j_query, * j_result, * j_return;
  char * username_escaped, * mod_name_escaped, * username_clause;
  int res;

  username_escaped = h_escape_string_with_quotes(config->conn, username);
  mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
  username_clause = msprintf(" = (SELECT gswu_id FROM "G_TABLE_WEBAUTHN_USER" WHERE UPPER(gswu_username) = UPPER(%s) AND gswu_mod_name = %s)", username_escaped, mod_name_escaped);
  j_query = json_pack("{sss[sss]s{sss{ssss}s{ssss}}}",
                      "table",
                      G_TABLE_WEBAUTHN_CREDENTIAL,
                      "columns",
                        "gswc_id",
                        "gswc_public_key AS public_key",
                        "gswc_counter AS counter",
                      "where",
                        "gswc_credential_id",
                        credential_id,
                        "gswu_id",
                          "operator",
                          "raw",
                          "value",
                          username_clause,
                        "gswc_status",
                          "operator",
                          "raw",
                          "value",
                          " IN (1,3)");
  o_free(username_clause);
  o_free(username_escaped);
  o_free(mod_name_escaped);
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result)) {
      j_return = json_pack("{sisO}", "result", G_OK, "credential", json_array_get(j_result, 0));
    } else {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_credential - Error executing j_query");
    config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
    j_return = json_pack("{si}", "result", G_ERROR_DB);
  }
  return j_return;
}