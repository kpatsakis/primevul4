check_uvalue (const unsigned char * start,
	      dwarf_vma             uvalue,
	      const unsigned char * end)
{
  dwarf_vma max_uvalue = end - start;

  /* See PR 17512: file: 008-103549-0.001:0.1.
     and PR 24829 for examples of where these tests are triggered.  */
  if (uvalue > max_uvalue)
    {
      warn (_("Corrupt attribute block length: %lx\n"), (long) uvalue);
      uvalue = max_uvalue;
    }

  return uvalue;
}