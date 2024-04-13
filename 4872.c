regname (unsigned int regno, int name_only_p)
{
  static char reg[64];

  const char *name = NULL;

  if (dwarf_regnames_lookup_func != NULL)
    name = dwarf_regnames_lookup_func (regno);

  if (name != NULL)
    {
      if (name_only_p)
	return name;
      snprintf (reg, sizeof (reg), "r%d (%s)", regno, name);
    }
  else
    snprintf (reg, sizeof (reg), "r%d", regno);
  return reg;
}