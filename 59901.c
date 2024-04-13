rend_service_non_anonymous_mode_consistent(const or_options_t *options)
{
  /* !! is used to make these options boolean */
  return (!! options->HiddenServiceSingleHopMode ==
          !! options->HiddenServiceNonAnonymousMode);
}
