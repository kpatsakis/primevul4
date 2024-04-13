add_abbrev (unsigned long  number,
	    unsigned long  tag,
	    int            children,
	    abbrev_list *  list)
{
  abbrev_entry *  entry;

  entry = (abbrev_entry *) xmalloc (sizeof (*entry));

  entry->number     = number;
  entry->tag        = tag;
  entry->children   = children;
  entry->first_attr = NULL;
  entry->last_attr  = NULL;
  entry->next       = NULL;

  assert (list != NULL);

  if (list->first_abbrev == NULL)
    list->first_abbrev = entry;
  else
    list->last_abbrev->next = entry;

  list->last_abbrev = entry;
}