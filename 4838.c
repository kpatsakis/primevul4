parse_gnu_debuglink (struct dwarf_section * section, void * data)
{
  const char *     name;
  unsigned int     crc_offset;
  unsigned long *  crc32 = (unsigned long *) data;

  /* The name is first.
     The CRC value is stored after the filename, aligned up to 4 bytes.  */
  name = (const char *) section->start;

  crc_offset = strnlen (name, section->size) + 1;
  if (crc_offset == 1)
    return NULL;
  crc_offset = (crc_offset + 3) & ~3;
  if (crc_offset + 4 > section->size)
    return NULL;

  * crc32 = byte_get (section->start + crc_offset, 4);
  return name;
}