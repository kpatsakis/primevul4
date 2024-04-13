rend_service_allow_non_anonymous_connection(const or_options_t *options)
{
  tor_assert(rend_service_non_anonymous_mode_consistent(options));
  return options->HiddenServiceSingleHopMode ? 1 : 0;
}
