get_type_abbrev_from_form (unsigned long form,
			   unsigned long uvalue,
			   dwarf_vma cu_offset,
			   unsigned char *cu_end,
			   const struct dwarf_section *section,
			   unsigned long *abbrev_num_return,
			   unsigned char **data_return,
			   abbrev_map **map_return)
{
  unsigned long   abbrev_number;
  abbrev_map *    map;
  abbrev_entry *  entry;
  unsigned char * data;

  if (abbrev_num_return != NULL)
    * abbrev_num_return = 0;
  if (data_return != NULL)
    * data_return = NULL;

  switch (form)
    {
    case DW_FORM_GNU_ref_alt:
    case DW_FORM_ref_sig8:
      /* FIXME: We are unable to handle this form at the moment.  */
      return NULL;

    case DW_FORM_ref_addr:
      if (uvalue >= section->size)
	{
	  warn (_("Unable to resolve ref_addr form: uvalue %lx > section size %lx (%s)\n"),
		uvalue, (long) section->size, section->name);
	  return NULL;
	}
      break;

    case DW_FORM_ref_sup4:
    case DW_FORM_ref_sup8:
      break;

    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref8:
    case DW_FORM_ref_udata:
      if (uvalue + cu_offset > (size_t) (cu_end - section->start))
	{
	  warn (_("Unable to resolve ref form: uvalue %lx + cu_offset %lx > CU size %lx\n"),
		uvalue, (long) cu_offset, (long) (cu_end - section->start));
	  return NULL;
	}
      uvalue += cu_offset;
      break;

      /* FIXME: Are there other DW_FORMs that can be used by types ?  */

    default:
      warn (_("Unexpected form %lx encountered whilst finding abbreviation for type\n"), form);
      return NULL;
    }

  data = (unsigned char *) section->start + uvalue;
  map = find_abbrev_map_by_offset (uvalue);

  if (map == NULL)
    {
      warn (_("Unable to find abbreviations for CU offset %#lx\n"), uvalue);
      return NULL;
    }
  if (map->list == NULL)
    {
      warn (_("Empty abbreviation list encountered for CU offset %lx\n"), uvalue);
      return NULL;
    }

  if (map_return != NULL)
    {
      if (form == DW_FORM_ref_addr)
	*map_return = map;
      else
	*map_return = NULL;
    }
	
  READ_ULEB (abbrev_number, data, section->start + section->size);

  for (entry = map->list->first_abbrev; entry != NULL; entry = entry->next)
    if (entry->number == abbrev_number)
      break;

  if (abbrev_num_return != NULL)
    * abbrev_num_return = abbrev_number;

  if (data_return != NULL)
    * data_return = data;

  if (entry == NULL)
    warn (_("Unable to find entry for abbreviation %lu\n"), abbrev_number);

  return entry;
}