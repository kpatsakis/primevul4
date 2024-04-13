get_IDX_name (unsigned long idx)
{
  const char *name = get_DW_IDX_name ((unsigned int) idx);

  if (name == NULL)
    {
      static char buffer[100];

      snprintf (buffer, sizeof (buffer), _("Unknown IDX value: %lx"), idx);
      return buffer;
    }

  return name;
}