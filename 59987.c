get_retry_schedule(time_t failing_since, time_t now,
                   int is_primary)
{
  const unsigned SIX_HOURS = 6 * 3600;
  const unsigned FOUR_DAYS = 4 * 86400;
  const unsigned SEVEN_DAYS = 7 * 86400;

  time_t tdiff;
  if (now > failing_since) {
    tdiff = now - failing_since;
  } else {
    tdiff = 0;
  }

  const struct {
    time_t maximum; int primary_delay; int nonprimary_delay;
  } delays[] = {
    { SIX_HOURS,    10*60,  1*60*60 },
    { FOUR_DAYS,    90*60,  4*60*60 },
    { SEVEN_DAYS, 4*60*60, 18*60*60 },
    { TIME_MAX,   9*60*60, 36*60*60 }
  };

  unsigned i;
  for (i = 0; i < ARRAY_LENGTH(delays); ++i) {
    if (tdiff <= delays[i].maximum) {
      return is_primary ? delays[i].primary_delay : delays[i].nonprimary_delay;
    }
  }
  /* LCOV_EXCL_START -- can't reach, since delays ends with TIME_MAX. */
  tor_assert_nonfatal_unreached();
  return 36*60*60;
  /* LCOV_EXCL_STOP */
}
