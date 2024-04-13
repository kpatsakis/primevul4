entry_list_is_constrained(const or_options_t *options)
{
  if (options->EntryNodes)
    return 1;
  if (options->UseBridges)
    return 1;
  return 0;
}
