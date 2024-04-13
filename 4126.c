static json_t * generate_credential_fake_from_seed(const char * seed) {
  unsigned char credential_id[64] = {0}, credential_id_b64[129], created_at[32], name_hash[32];
  char * seed_credential_id, * seed_name, * seed_created_at, name[32];
  time_t created_at_t;
  size_t credential_id_len = 64, credential_id_b64_len, name_hash_len = 32, created_at_len = 32;
  json_t * j_return;

  if ((seed_credential_id = msprintf("%s-credential_id", seed)) != NULL) {
    if (generate_digest_raw(digest_SHA512, (unsigned char *)seed_credential_id, o_strlen(seed_credential_id), credential_id, &credential_id_len)) {
      if (o_base64_encode(credential_id, credential_id_len, credential_id_b64, &credential_id_b64_len)) {
        if ((seed_name = msprintf("%s-name", seed)) != NULL) {
          if (generate_digest_raw(digest_SHA256, (unsigned char *)seed_name, o_strlen(seed_name), name_hash, &name_hash_len)) {
            if (name_hash[0]%2) {
              o_strcpy(name, "fido-u2f");
            } else {
              o_strcpy(name, "android-safetynet");
            }
            if ((seed_created_at = msprintf("%s-created_at", seed)) != NULL) {
              if (generate_digest_raw(digest_SHA256, (unsigned char *)seed_created_at, o_strlen(seed_created_at), created_at, &created_at_len)) {
                time(&created_at_t);
                created_at_t -= created_at[0] - (created_at[1] << 8);
                j_return = json_pack("{sis{sssssiss}}",
                                     "result",
                                     G_OK,
                                     "credential",
                                      "credential_id",
                                      credential_id_b64,
                                      "name",
                                      name,
                                      "created_at",
                                      created_at_t,
                                      "status",
                                      "registered");
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error generate_digest_raw for seed_created_at");
                j_return = json_pack("{si}", "result", G_ERROR);
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error allocating resources for seed_created_at");
              j_return = json_pack("{si}", "result", G_ERROR_MEMORY);
            }
            o_free(seed_created_at);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error generate_digest_raw for seed_name");
            j_return = json_pack("{si}", "result", G_ERROR);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error allocating resources for seed_name");
          j_return = json_pack("{si}", "result", G_ERROR_MEMORY);
        }
        o_free(seed_name);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error o_base64_encode for seed_credential_id");
        j_return = json_pack("{si}", "result", G_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error generate_digest_raw for seed_credential_id");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error allocating resources for seed_credential_id");
    j_return = json_pack("{si}", "result", G_ERROR_MEMORY);
  }
  o_free(seed_credential_id);
  return j_return;
}