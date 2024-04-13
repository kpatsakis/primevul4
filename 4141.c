static int generate_fake_user_id(json_t * j_params, const char * username, unsigned char * user_id) {
  char * seed;
  unsigned char seed_hash[32];
  size_t seed_hash_len = 32, seed_hash_b64_len;
  int ret;

  if ((seed = msprintf("%s%s-user_id", username, json_string_value(json_object_get(j_params, "seed")))) != NULL) {
    if (generate_digest_raw(digest_SHA256, (unsigned char *)seed, o_strlen(seed), seed_hash, &seed_hash_len)) {
      if (o_base64_encode(seed_hash, seed_hash_len, user_id, &seed_hash_b64_len)) {
        ret = G_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error o_base64_encode");
        ret = G_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error generate_digest_raw");
      ret = G_ERROR;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_credential_fake_from_seed - Error allocating resources for seed");
    ret = G_ERROR_MEMORY;
  }
  o_free(seed);
  return ret;
}