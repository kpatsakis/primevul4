static int check_certificate(struct config_module * config, json_t * j_params, const char * credential_id, json_int_t gswu_id) {
  json_t * j_query, * j_result;
  int res, ret;
  char * credential_id_escaped, * mod_name_escaped, * where_clause;

  credential_id_escaped = h_escape_string_with_quotes(config->conn, credential_id);
  mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
  where_clause = msprintf(" IN (SELECT gswu_id FROM " G_TABLE_WEBAUTHN_CREDENTIAL " WHERE gswc_credential_id=%s AND gswc_status=1 AND gswu_id IN (SELECT gswu_id FROM " G_TABLE_WEBAUTHN_USER " WHERE gswu_mod_name=%s))", credential_id_escaped, mod_name_escaped);
  j_query = json_pack("{sss[s]s{s{ssss}si}}",
                      "table",
                      G_TABLE_WEBAUTHN_CREDENTIAL,
                      "columns",
                        "gswu_id",
                      "where",
                        "gswu_id",
                          "operator",
                          "raw",
                          "value",
                          where_clause,
                        "gswc_status",
                        1);
  o_free(where_clause);
  o_free(mod_name_escaped);
  o_free(credential_id_escaped);
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result)) {
      if (json_integer_value(json_object_get(json_array_get(j_result, 0), "gswu_id")) == gswu_id) {
        ret = G_OK;
      } else {
        ret = G_ERROR_UNAUTHORIZED;
      }
    } else {
      ret = G_ERROR_NOT_FOUND;
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "check_credential_id - Error executing j_query");
    config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
    ret = G_ERROR_DB;
  }
  return ret;
}