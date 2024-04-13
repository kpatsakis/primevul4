rend_service_poison_new_single_onion_dir(const rend_service_t *s,
                                         const or_options_t* options)
{
  /* Passing a NULL service is a bug */
  if (BUG(!s)) {
    return -1;
  }

  /* We must only poison directories if we're in Single Onion mode */
  tor_assert(rend_service_non_anonymous_mode_enabled(options));

  /* Ephemeral services aren't allowed in non-anonymous mode */
  if (BUG(rend_service_is_ephemeral(s))) {
    return -1;
  }

  /* Service is expected to have a directory */
  if (BUG(!s->directory)) {
    return -1;
  }

  if (!rend_service_private_key_exists(s)) {
    if (poison_new_single_onion_hidden_service_dir_impl(s, options)
        < 0) {
      return -1;
    }
  }

  return 0;
}
