get_type_signedness (abbrev_entry *entry,
		     const struct dwarf_section *section,
		     unsigned char *data,
		     unsigned char *end,
		     dwarf_vma cu_offset,
		     dwarf_vma pointer_size,
		     dwarf_vma offset_size,
		     int dwarf_version,
		     bool *is_signed,
		     unsigned int nesting)
{
  abbrev_attr *   attr;

  * is_signed = false;

#define MAX_NESTING 20
  if (nesting > MAX_NESTING)
    {
      /* FIXME: Warn - or is this expected ?
	 NB/ We need to avoid infinite recursion.  */
      return;
    }

  for (attr = entry->first_attr;
       attr != NULL && attr->attribute;
       attr = attr->next)
    {
      unsigned char * orig_data = data;
      dwarf_vma uvalue = 0;

      data = skip_attr_bytes (attr->form, data, end, pointer_size,
			      offset_size, dwarf_version, & uvalue);
      if (data == NULL)
	return;

      switch (attr->attribute)
	{
	case DW_AT_linkage_name:
	case DW_AT_name:
	  if (do_wide)
	    {
	      if (attr->form == DW_FORM_strp)
		printf (", %s", fetch_indirect_string (uvalue));
	      else if (attr->form == DW_FORM_string)
		printf (", %.*s", (int) (end - orig_data), orig_data);
	    }
	  break;

	case DW_AT_type:
	  /* Recurse.  */
	  {
	    abbrev_entry *type_abbrev;
	    unsigned char *type_data;
	    abbrev_map *map;

	    type_abbrev = get_type_abbrev_from_form (attr->form,
						     uvalue,
						     cu_offset,
						     end,
						     section,
						     NULL /* abbrev num return */,
						     &type_data,
						     &map);
	    if (type_abbrev == NULL)
	      break;

	    get_type_signedness (type_abbrev, section, type_data,
				 map ? section->start + map->end : end,
				 map ? map->start : cu_offset,
				 pointer_size, offset_size, dwarf_version,
				 is_signed, nesting + 1);
	  }
	  break;

	case DW_AT_encoding:
	  /* Determine signness.  */
	  switch (uvalue)
	    {
	    case DW_ATE_address:
	      /* FIXME - some architectures have signed addresses.  */
	    case DW_ATE_boolean:
	    case DW_ATE_unsigned:
	    case DW_ATE_unsigned_char:
	    case DW_ATE_unsigned_fixed:
	      * is_signed = false;
	      break;

	    default:
	    case DW_ATE_complex_float:
	    case DW_ATE_float:
	    case DW_ATE_signed:
	    case DW_ATE_signed_char:
	    case DW_ATE_imaginary_float:
	    case DW_ATE_decimal_float:
	    case DW_ATE_signed_fixed:
	      * is_signed = true;
	      break;
	    }
	  break;
	}
    }
}