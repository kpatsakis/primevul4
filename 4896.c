new_abbrev_list (dwarf_vma abbrev_base, dwarf_vma abbrev_offset)
{
  abbrev_list * list = (abbrev_list *) xcalloc (sizeof * list, 1);

  list->abbrev_base = abbrev_base;
  list->abbrev_offset = abbrev_offset;

  list->next = abbrev_lists;
  abbrev_lists = list;

  return list;
}