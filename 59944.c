entry_guard_has_higher_priority(entry_guard_t *a, entry_guard_t *b)
{
  tor_assert(a && b);
  if (a == b)
    return 0;

  /* Confirmed is always better than unconfirmed; lower index better
     than higher */
  if (a->confirmed_idx < 0) {
    if (b->confirmed_idx >= 0)
      return 0;
  } else {
    if (b->confirmed_idx < 0)
      return 1;

    /* Lower confirmed_idx is better than higher. */
    return (a->confirmed_idx < b->confirmed_idx);
  }

  /* If we reach this point, both are unconfirmed. If one is pending, it
   * has higher priority. */
  if (a->is_pending) {
    if (! b->is_pending)
      return 1;

    /* Both are pending: earlier last_tried_connect wins. */
    return a->last_tried_to_connect < b->last_tried_to_connect;
  } else {
    if (b->is_pending)
      return 0;

    /* Neither is pending: priorities are equal. */
    return 0;
  }
}
