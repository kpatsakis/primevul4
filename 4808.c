display_discr_list (unsigned long          form,
		    dwarf_vma              uvalue,
		    unsigned char *        data,
		    int                    level)
{
  unsigned char *end = data;

  if (uvalue == 0)
    {
      printf ("[default]");
      return;
    }

  switch (form)
    {
    case DW_FORM_block:
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
      /* Move data pointer back to the start of the byte array.  */
      data -= uvalue;
      break;
    default:
      printf ("<corrupt>\n");
      warn (_("corrupt discr_list - not using a block form\n"));
      return;
    }

  if (uvalue < 2)
    {
      printf ("<corrupt>\n");
      warn (_("corrupt discr_list - block not long enough\n"));
      return;
    }

  bool is_signed = (level > 0 && level <= MAX_CU_NESTING
		    ? level_type_signed [level - 1] : false);

  printf ("(");
  while (data < end)
    {
      unsigned char     discriminant;
      unsigned int      bytes_read;

      SAFE_BYTE_GET_AND_INC (discriminant, data, 1, end);

      switch (discriminant)
	{
	case DW_DSC_label:
	  printf ("label ");
	  read_and_print_leb128 (data, & bytes_read, end, is_signed);
	  data += bytes_read;
	  break;

	case DW_DSC_range:
	  printf ("range ");
	  read_and_print_leb128 (data, & bytes_read, end, is_signed);
	  data += bytes_read;

	  printf ("..");
	  read_and_print_leb128 (data, & bytes_read, end, is_signed);
	  data += bytes_read;
	  break;

	default:
	  printf ("<corrupt>\n");
	  warn (_("corrupt discr_list - unrecognized discriminant byte %#x\n"),
		discriminant);
	  return;
	}

      if (data < end)
	printf (", ");
    }

  if (is_signed)
    printf (")(signed)");
  else
    printf (")(unsigned)");
}