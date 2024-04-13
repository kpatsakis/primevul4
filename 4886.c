free_all_abbrevs (void)
{
  abbrev_list *  list;

  for (list = abbrev_lists; list != NULL;)
    {
      abbrev_list *   next = list->next;
      abbrev_entry *  abbrv;

      for (abbrv = list->first_abbrev; abbrv != NULL;)
	{
	  abbrev_entry *  next_abbrev = abbrv->next;
	  abbrev_attr *   attr;

	  for (attr = abbrv->first_attr; attr;)
	    {
	      abbrev_attr *next_attr = attr->next;

	      free (attr);
	      attr = next_attr;
	    }

	  free (abbrv);
	  abbrv = next_abbrev;
	}

      free (list);
      list = next;
    }

  abbrev_lists = NULL;
}