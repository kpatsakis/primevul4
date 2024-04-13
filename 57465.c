int _yr_scan_add_match_to_list(
    YR_MATCH* match,
    YR_MATCHES* matches_list,
    int replace_if_exists)
{
  YR_MATCH* insertion_point = matches_list->tail;

  if (matches_list->count == MAX_STRING_MATCHES)
    return ERROR_TOO_MANY_MATCHES;

  while (insertion_point != NULL)
  {
    if (match->offset == insertion_point->offset)
    {
      if (replace_if_exists)
      {
        insertion_point->match_length = match->match_length;
        insertion_point->data_length = match->data_length;
        insertion_point->data = match->data;
      }

      return ERROR_SUCCESS;
    }

    if (match->offset > insertion_point->offset)
      break;

    insertion_point = insertion_point->prev;
  }

  match->prev = insertion_point;

  if (insertion_point != NULL)
  {
    match->next = insertion_point->next;
    insertion_point->next = match;
  }
  else
  {
    match->next = matches_list->head;
    matches_list->head = match;
  }

  matches_list->count++;

  if (match->next != NULL)
    match->next->prev = match;
  else
    matches_list->tail = match;

  return ERROR_SUCCESS;
}
