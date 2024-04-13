display_block (unsigned char *data,
	       dwarf_vma length,
	       const unsigned char * const end, char delimiter)
{
  dwarf_vma maxlen;

  printf (_("%c%s byte block: "), delimiter, dwarf_vmatoa ("u", length));
  if (data > end)
    return (unsigned char *) end;

  maxlen = (dwarf_vma) (end - data);
  length = length > maxlen ? maxlen : length;

  while (length --)
    printf ("%lx ", (unsigned long) byte_get (data++, 1));

  return data;
}