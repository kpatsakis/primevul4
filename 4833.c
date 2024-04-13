display_debug_str (struct dwarf_section *section,
		   void *file ATTRIBUTE_UNUSED)
{
  unsigned char *start = section->start;
  dwarf_vma bytes = section->size;
  dwarf_vma addr = section->address;

  if (bytes == 0)
    {
      printf (_("\nThe %s section is empty.\n"), section->name);
      return 0;
    }

  introduce (section, false);

  while (bytes)
    {
      int j;
      int k;
      int lbytes;

      lbytes = (bytes > 16 ? 16 : bytes);

      printf ("  0x%8.8lx ", (unsigned long) addr);

      for (j = 0; j < 16; j++)
	{
	  if (j < lbytes)
	    printf ("%2.2x", start[j]);
	  else
	    printf ("  ");

	  if ((j & 3) == 3)
	    printf (" ");
	}

      for (j = 0; j < lbytes; j++)
	{
	  k = start[j];
	  if (k >= ' ' && k < 0x80)
	    printf ("%c", k);
	  else
	    printf (".");
	}

      putchar ('\n');

      start += lbytes;
      addr  += lbytes;
      bytes -= lbytes;
    }

  putchar ('\n');

  return 1;
}