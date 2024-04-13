rend_service_verify_single_onion_poison(const rend_service_t* s,
                                        const or_options_t* options)
{
  /* Passing a NULL service is a bug */
  if (BUG(!s)) {
    return -1;
  }

  /* Ephemeral services are checked at ADD_ONION time */
  if (BUG(rend_service_is_ephemeral(s))) {
    return -1;
  }

  /* Service is expected to have a directory */
  if (BUG(!s->directory)) {
    return -1;
  }

  /* Services without keys are always ok - their keys will only ever be used
   * in the current mode */
  if (!rend_service_private_key_exists(s)) {
    return 0;
  }

  /* The key has been used before in a different mode */
  if (service_is_single_onion_poisoned(s) !=
      rend_service_non_anonymous_mode_enabled(options)) {
    return -1;
  }

  /* The key exists and is consistent with the current mode */
  return 0;
}
