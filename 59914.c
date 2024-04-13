rend_service_reveal_startup_time(const or_options_t *options)
{
  tor_assert(rend_service_non_anonymous_mode_consistent(options));
  return rend_service_non_anonymous_mode_enabled(options);
}
