static json_t * generate_new_credential(struct config_module * config, json_t * j_params, const char * username) {
  json_t * j_query, * j_return;
  char * username_escaped, * mod_name_escaped, * username_clause, * challenge_hash;
  int res;
  size_t challenge_b64_len, challenge_len = (size_t)json_integer_value(json_object_get(j_params, "challenge-length"));
  unsigned char challenge_b64[challenge_len*2], challenge[challenge_len+1];
  char session[SESSION_LENGTH+1] = {0}, * session_hash;

  gnutls_rnd(GNUTLS_RND_NONCE, challenge, challenge_len);
  if (o_base64_encode(challenge, challenge_len, challenge_b64, &challenge_b64_len)) {
    challenge_b64[challenge_b64_len] = '\0';
    if ((challenge_hash = generate_hash(config->hash_algorithm, (const char *)challenge_b64)) != NULL) {
      rand_string(session, SESSION_LENGTH);
      if ((session_hash = generate_hash(config->hash_algorithm, session)) != NULL) {
        username_escaped = h_escape_string_with_quotes(config->conn, username);
        mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
        username_clause = msprintf(" (SELECT gswu_id FROM "G_TABLE_WEBAUTHN_USER" WHERE UPPER(gswu_username) = UPPER(%s) AND gswu_mod_name = %s)", username_escaped, mod_name_escaped);
        // Disable all credential with status 0 (new) of the same user
        j_query = json_pack("{sss{si}s{s{ssss+}si}}",
                            "table",
                            G_TABLE_WEBAUTHN_CREDENTIAL,
                            "set",
                              "gswc_status",
                              2,
                            "where",
                              "gswu_id",
                                "operator",
                                "raw",
                                "value",
                                " =",
                                username_clause,
                              "gswc_status",
                              0);
        res = h_update(config->conn, j_query, NULL);
        json_decref(j_query);
        if (res == H_OK) {
          // Insert new credential
          j_query = json_pack("{sss{s{ss}sssssi}}",
                              "table",
                              G_TABLE_WEBAUTHN_CREDENTIAL,
                              "values",
                                "gswu_id",
                                  "raw",
                                  username_clause,
                                "gswc_session_hash",
                                session_hash,
                                "gswc_challenge_hash",
                                challenge_hash,
                                "gswc_status",
                                0);
          res = h_insert(config->conn, j_query, NULL);
          json_decref(j_query);
          if (res == H_OK) {
            j_return = json_pack("{sis{ssss}}", "result", G_OK, "credential", "session", session, "challenge", challenge_b64);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_credential - Error executing j_query insert");
            config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
            j_return = json_pack("{si}", "result", G_ERROR_DB);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_credential - Error executing j_query update");
          config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
          j_return = json_pack("{si}", "result", G_ERROR_DB);
        }
        o_free(username_clause);
        o_free(username_escaped);
        o_free(mod_name_escaped);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_credential - Error generate_hash session");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      o_free(session_hash);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_credential - Error generate_hash challenge");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    o_free(challenge_hash);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_credential - Error o_base64_encode challenge");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  return j_return;
}