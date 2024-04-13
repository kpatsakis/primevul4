get_FORM_name (unsigned long form)
{
  const char *name;

  if (form == 0)
    return "DW_FORM value: 0";

  name = get_DW_FORM_name (form);
  if (name == NULL)
    {
      static char buffer[100];

      snprintf (buffer, sizeof (buffer), _("Unknown FORM value: %lx"), form);
      return buffer;
    }

  return name;
}