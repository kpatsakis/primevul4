static json_t * get_credential_list(struct config_module * config, json_t * j_params, const char * username, int restrict_to_registered) {
  json_t * j_query, * j_result, * j_return, * j_element = NULL;
  int res;
  char * username_escaped, * mod_name_escaped, * username_clause;
  size_t index = 0;

  username_escaped = h_escape_string_with_quotes(config->conn, username);
  mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
  username_clause = msprintf(" = (SELECT gswu_id FROM "G_TABLE_WEBAUTHN_USER" WHERE UPPER(gswu_username) = UPPER(%s) AND gswu_mod_name = %s)", username_escaped, mod_name_escaped);
  j_query = json_pack("{sss[ssss]s{s{ssss}}}",
                      "table",
                      G_TABLE_WEBAUTHN_CREDENTIAL,
                      "columns",
                        "gswc_credential_id AS credential_id",
                        "gswc_name AS name",
                        SWITCH_DB_TYPE(config->conn->type, "UNIX_TIMESTAMP(gswc_created_at) AS created_at", "strftime('%s', gswc_created_at) AS created_at", "EXTRACT(EPOCH FROM gswc_created_at)::integer AS created_at"),
                        "gswc_status",
                      "where",
                        "gswu_id",
                          "operator",
                          "raw",
                          "value",
                          username_clause);
  o_free(username_clause);
  o_free(username_escaped);
  o_free(mod_name_escaped);
  if (restrict_to_registered) {
    json_object_set_new(json_object_get(j_query, "where"), "gswc_status", json_integer(1));
  } else {
    json_object_set_new(json_object_get(j_query, "where"), "gswc_status", json_pack("{ssss}", "operator", "raw", "value", " IN (1,3)"));
  }
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result)) {
      j_return = json_pack("{sis[]}", "result", G_OK, "credential");
      if (j_return != NULL) {
        json_array_foreach(j_result, index, j_element) {
          switch (json_integer_value(json_object_get(j_element, "gswc_status"))) {
            case 1:
              json_object_set_new(j_element, "status", json_string("registered"));
              break;
            case 3:
              json_object_set_new(j_element, "status", json_string("disabled"));
              break;
            default:
              break;
          }
          json_object_del(j_element, "gswc_status");
          json_array_append(json_object_get(j_return, "credential"), j_element);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_credential_list - Error json_pack");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
    } else {
      j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_credential_list - Error executing j_query");
    config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
    j_return = json_pack("{si}", "result", G_ERROR_DB);
  }
  return j_return;
}