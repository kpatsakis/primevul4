rend_service_non_anonymous_mode_enabled(const or_options_t *options)
{
  tor_assert(rend_service_non_anonymous_mode_consistent(options));
  return options->HiddenServiceNonAnonymousMode ? 1 : 0;
}
