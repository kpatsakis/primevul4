void _yr_scan_update_match_chain_length(
    int tidx,
    YR_STRING* string,
    YR_MATCH* match_to_update,
    int chain_length)
{
  YR_MATCH* match;

  if (match_to_update->chain_length == chain_length)
    return;

  match_to_update->chain_length = chain_length;

  if (string->chained_to == NULL)
    return;

  match = string->chained_to->unconfirmed_matches[tidx].head;

  while (match != NULL)
  {
    int64_t ending_offset = match->offset + match->match_length;

    if (ending_offset + string->chain_gap_max >= match_to_update->offset &&
        ending_offset + string->chain_gap_min <= match_to_update->offset)
    {
      _yr_scan_update_match_chain_length(
          tidx, string->chained_to, match, chain_length + 1);
    }

    match = match->next;
  }
}
