add_abbrev_attr (unsigned long    attribute,
		 unsigned long    form,
		 dwarf_signed_vma implicit_const,
		 abbrev_list *    list)
{
  abbrev_attr *attr;

  attr = (abbrev_attr *) xmalloc (sizeof (*attr));

  attr->attribute      = attribute;
  attr->form           = form;
  attr->implicit_const = implicit_const;
  attr->next           = NULL;

  assert (list != NULL && list->last_abbrev != NULL);

  if (list->last_abbrev->first_attr == NULL)
    list->last_abbrev->first_attr = attr;
  else
    list->last_abbrev->last_attr->next = attr;

  list->last_abbrev->last_attr = attr;
}