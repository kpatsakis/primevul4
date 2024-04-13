get_AT_name (unsigned long attribute)
{
  const char *name;

  if (attribute == 0)
    return "DW_AT value: 0";

  /* One value is shared by the MIPS and HP extensions:  */
  if (attribute == DW_AT_MIPS_fde)
    return "DW_AT_MIPS_fde or DW_AT_HP_unmodifiable";

  name = get_DW_AT_name (attribute);

  if (name == NULL)
    {
      static char buffer[100];

      snprintf (buffer, sizeof (buffer), _("Unknown AT value: %lx"),
		attribute);
      return buffer;
    }

  return name;
}