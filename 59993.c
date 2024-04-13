guard_get_guardfraction_bandwidth(guardfraction_bandwidth_t *guardfraction_bw,
                                  int orig_bandwidth,
                                  uint32_t guardfraction_percentage)
{
  double guardfraction_fraction;

  /* Turn the percentage into a fraction. */
  tor_assert(guardfraction_percentage <= 100);
  guardfraction_fraction = guardfraction_percentage / 100.0;

  long guard_bw = tor_lround(guardfraction_fraction * orig_bandwidth);
  tor_assert(guard_bw <= INT_MAX);

  guardfraction_bw->guard_bw = (int) guard_bw;

  guardfraction_bw->non_guard_bw = orig_bandwidth - (int) guard_bw;
}
