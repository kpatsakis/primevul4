intro_point_should_expire_now(rend_intro_point_t *intro,
                              time_t now)
{
  tor_assert(intro != NULL);

  if (intro->time_published == -1) {
    /* Don't expire an intro point if we haven't even published it yet. */
    return 0;
  }

  if (intro_point_accepted_intro_count(intro) >=
      intro->max_introductions) {
    /* This intro point has been used too many times.  Expire it now. */
    return 1;
  }

  if (intro->time_to_expire == -1) {
    /* This intro point has been published, but we haven't picked an
     * expiration time for it.  Pick one now. */
    int intro_point_lifetime_seconds =
      crypto_rand_int_range(INTRO_POINT_LIFETIME_MIN_SECONDS,
                            INTRO_POINT_LIFETIME_MAX_SECONDS);

    /* Start the expiration timer now, rather than when the intro
     * point was first published.  There shouldn't be much of a time
     * difference. */
    intro->time_to_expire = now + intro_point_lifetime_seconds;

    return 0;
  }

  /* This intro point has a time to expire set already.  Use it. */
  return (now >= intro->time_to_expire);
}
