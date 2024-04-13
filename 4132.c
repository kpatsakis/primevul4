static json_t * generate_new_assertion(struct config_module * config, json_t * j_params, const char * username, int mock) {
  json_t * j_query, * j_return;
  char * username_escaped, * username_clause, * mod_name_escaped, * challenge_hash;
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
        if (mock < 2) {
          username_escaped = h_escape_string_with_quotes(config->conn, username);
          mod_name_escaped = h_escape_string_with_quotes(config->conn, json_string_value(json_object_get(j_params, "mod_name")));
          username_clause = msprintf(" (SELECT gswu_id FROM "G_TABLE_WEBAUTHN_USER" WHERE UPPER(gswu_username) = UPPER(%s) AND gswu_mod_name = %s)", username_escaped, mod_name_escaped);
          // Disable all assertions with status 0 (new) of the same user
          j_query = json_pack("{sss{si}s{s{ssss+}si}}",
                              "table",
                              G_TABLE_WEBAUTHN_ASSERTION,
                              "set",
                                "gswa_status",
                                3,
                              "where",
                                "gswu_id",
                                  "operator",
                                  "raw",
                                  "value",
                                  " =",
                                  username_clause,
                                "gswa_status",
                                0);
          res = h_update(config->conn, j_query, NULL);
          json_decref(j_query);
          if (res == H_OK) {
            // Insert new assertion
            j_query = json_pack("{sss{s{ss}sssssisi}}",
                                "table",
                                G_TABLE_WEBAUTHN_ASSERTION,
                                "values",
                                  "gswu_id",
                                    "raw",
                                    username_clause,
                                  "gswa_session_hash",
                                  session_hash,
                                  "gswa_challenge_hash",
                                  challenge_hash,
                                  "gswa_status",
                                  0,
                                  "gswa_mock",
                                  mock);
            res = h_insert(config->conn, j_query, NULL);
            json_decref(j_query);
            if (res == H_OK) {
              j_return = json_pack("{sis{ssss}}", "result", G_OK, "assertion", "session", session, "challenge", challenge_b64);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_assertion - Error executing j_query insert");
              config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
              j_return = json_pack("{si}", "result", G_ERROR_DB);
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_assertion - Error executing j_query update");
            config->glewlwyd_module_callback_metrics_increment_counter(config, GLWD_METRICS_DATABSE_ERROR, 1, NULL);
            j_return = json_pack("{si}", "result", G_ERROR_DB);
          }
          o_free(username_clause);
          o_free(mod_name_escaped);
          o_free(username_escaped);
        } else {
          j_return = json_pack("{sis{ssss}}", "result", G_OK, "assertion", "session", session, "challenge", challenge_b64);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_assertion - Error generate_hash session");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
      o_free(session_hash);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_assertion - Error generate_hash challenge");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    o_free(challenge_hash);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_new_assertion - Error o_base64_encode challenge");
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  return j_return;
}