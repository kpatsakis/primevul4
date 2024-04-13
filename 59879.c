rend_max_intro_circs_per_period(unsigned int n_intro_points_wanted)
{
  /* Allow all but one of the initial connections to fail and be
   * retried. (If all fail, we *want* to wait, because something is broken.) */
  tor_assert(n_intro_points_wanted <= NUM_INTRO_POINTS_MAX);
  return (int)(2*n_intro_points_wanted + NUM_INTRO_POINTS_EXTRA);
}
