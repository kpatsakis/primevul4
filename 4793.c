introduce (struct dwarf_section * section, bool raw)
{
  if (raw)
    {
      if (do_follow_links && section->filename)
	printf (_("Raw dump of debug contents of section %s (loaded from %s):\n\n"),
		section->name, section->filename);
      else
	printf (_("Raw dump of debug contents of section %s:\n\n"), section->name);
    }
  else
    {
      if (do_follow_links && section->filename)
	printf (_("Contents of the %s section (loaded from %s):\n\n"),
		section->name, section->filename);
      else
	printf (_("Contents of the %s section:\n\n"), section->name);
    }
}