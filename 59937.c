entry_guard_consider_retry(entry_guard_t *guard)
{
  if (guard->is_reachable != GUARD_REACHABLE_NO)
    return; /* No retry needed. */

  const time_t now = approx_time();
  const int delay =
    get_retry_schedule(guard->failing_since, now, guard->is_primary);
  const time_t last_attempt = guard->last_tried_to_connect;

  if (BUG(last_attempt == 0) ||
      now >= last_attempt + delay) {
    /* We should mark this retriable. */
    char tbuf[ISO_TIME_LEN+1];
    format_local_iso_time(tbuf, last_attempt);
    log_info(LD_GUARD, "Marked %s%sguard %s for possible retry, since we "
             "haven't tried to use it since %s.",
             guard->is_primary?"primary ":"",
             guard->confirmed_idx>=0?"confirmed ":"",
             entry_guard_describe(guard),
             tbuf);

    guard->is_reachable = GUARD_REACHABLE_MAYBE;
    if (guard->is_filtered_guard)
      guard->is_usable_filtered_guard = 1;
  }
}
