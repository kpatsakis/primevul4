static json_t * generate_credential_fake_list(json_t * j_params, const char * username) {
  json_t * j_credential, * j_credential_sub, * j_return;
  char * seed;
  unsigned char seed_hash[32] = {0};
  size_t seed_hash_len = 32;
  unsigned int i;

  if ((seed = msprintf("%s%s0", username, json_string_value(json_object_get(j_params, "seed")))) != NULL) {
    j_credential = generate_credential_fake_from_seed(seed);
    if (check_result_value(j_credential, G_OK)) {
      j_return = json_pack("{sis[O]}", "result", G_OK, "credential", json_object_get(j_credential, "credential"));
      if (j_return != NULL) {
        if (generate_digest_raw(digest_SHA256, (unsigned char *)seed, o_strlen(seed), seed_hash, &seed_hash_len)) {
          for (i=0; i<seed_hash[0]%3; i++) {
            seed[o_strlen(seed)-1]++;
            j_credential_sub = generate_credential_fake_from_seed(seed);
            if (check_result_value(j_credential, G_OK)) {
              json_array_append(json_object_get(j_return, "credential"), json_object_get(j_credential_sub, "credential"));
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_list - Error generate_credential_fake_from_seed at index %u", i);
            }
            json_decref(j_credential_sub);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_list - Error generate_digest_raw");
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_list - Error allocating resources for j_return");
        j_return = json_pack("{si}", "result", G_ERROR_MEMORY);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_list - Error generate_credential_fake_from_seed");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
    json_decref(j_credential);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_list - Error allocating resources for seed");
    j_return = json_pack("{si}", "result", G_ERROR_MEMORY);
  }
  o_free(seed);
  return j_return;
}