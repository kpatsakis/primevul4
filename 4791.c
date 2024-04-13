get_TAG_name (unsigned long tag)
{
  const char *name = get_DW_TAG_name ((unsigned int) tag);

  if (name == NULL)
    {
      static char buffer[100];

      if (tag >= DW_TAG_lo_user && tag <= DW_TAG_hi_user)
	snprintf (buffer, sizeof (buffer), _("User TAG value: %#lx"), tag);
      else
	snprintf (buffer, sizeof (buffer), _("Unknown TAG value: %#lx"), tag);
      return buffer;
    }

  return name;
}