static json_t * get_assertion_from_session(struct config_module * config, json_t * j_params, const char * username, const char * session, int mock) {
  json_t * j_query, * j_result, * j_return;
  char * username_escaped, * mod_name_escaped, * username_clause, * expiration_clause;
  char * session_hash;
  int res;
  time_t now;

  if (o_strlen(session)) {
    session_hash = generate_hash(config->hash_algorithm, session);
    if (session_hash != NULL) {
      time(&now);
      username_escaped = h_escape_string_with_quotes(config->conn, username);
      mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
      username_clause = msprintf(" = (SELECT gswu_id FROM "G_TABLE_WEBAUTHN_USER" WHERE UPPER(gswu_username) = UPPER(%s) AND gswu_mod_name = %s)", username_escaped, mod_name_escaped);
      if (config->conn->type==HOEL_DB_TYPE_MARIADB) {
        expiration_clause = msprintf("> FROM_UNIXTIME(%u)", (now - (unsigned int)json_integer_value(json_object_get(j_params, "credential-assertion"))));
      } else if (config->conn->type==HOEL_DB_TYPE_PGSQL) {
        expiration_clause = msprintf("> TO_TIMESTAMP(%u)", (now - (unsigned int)json_integer_value(json_object_get(j_params, "credential-assertion"))));
      } else { // HOEL_DB_TYPE_SQLITE
        expiration_clause = msprintf("> %u", (now - (unsigned int)json_integer_value(json_object_get(j_params, "credential-assertion"))));
      }
      j_query = json_pack("{sss[ssss]s{sss{ssss}sis{ssss}si}}",
                          "table",
                          G_TABLE_WEBAUTHN_ASSERTION,
                          "columns",
                            "gswa_id",
                            "gswu_id",
                            "gswa_session_hash AS session_hash",
                            "gswa_challenge_hash AS challenge_hash",
                          "where",
                            "gswa_session_hash",
                            session_hash,
                            "gswu_id",
                              "operator",
                              "raw",
                              "value",
                              username_clause,
                            "gswa_status",
                            0,
                            "gswa_issued_at",
                              "operator",
                              "raw",
                              "value",
                              expiration_clause,
                            "gswa_mock",
                            mock);
      o_free(username_clause);
      o_free(username_escaped);
      o_free(mod_name_escaped);
      o_free(expiration_clause);
      res = h_select(config->conn, j_query, &j_result, NULL);
      json_decref(j_query);
      if (res == H_OK) {
        if (json_array_size(j_result)) {
          j_return = json_pack("{sisO}", "result", G_OK, "assertion", json_array_get(j_result, 0));
        } else {
          j_return = json_pack("{si}", "result", G_ERROR_NOT_FOUND);
        }
        json_decref(j_result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_assertion_from_session - Error executing j_query");
        config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
        j_return = json_pack("{si}", "result", G_ERROR_DB);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_assertion_from_session - Error generate_hash");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    o_free(session_hash);
  } else {
    j_return = json_pack("{si}", "result", G_ERROR_PARAM);
  }
  return j_return;
}