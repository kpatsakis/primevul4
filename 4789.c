load_build_id_debug_file (const char * main_filename ATTRIBUTE_UNUSED, void * main_file)
{
  if (! load_debug_section (note_gnu_build_id, main_file))
    return; /* No .note.gnu.build-id section.  */

  struct dwarf_section * section = & debug_displays [note_gnu_build_id].section;
  if (section == NULL)
    {
      warn (_("Unable to load the .note.gnu.build-id section\n"));
      return;
    }

  if (section->start == NULL || section->size < 0x18)
    {
      warn (_(".note.gnu.build-id section is corrupt/empty\n"));
      return;
    }

  /* In theory we should extract the contents of the section into
     a note structure and then check the fields.  For now though
     just use hard coded offsets instead:
     
       Field  Bytes    Contents
	NSize  0...3   4
	DSize  4...7   8+
	Type   8..11   3  (NT_GNU_BUILD_ID)
        Name   12.15   GNU\0
	Data   16....   */

  /* FIXME: Check the name size, name and type fields.  */

  unsigned long build_id_size;
  build_id_size = byte_get (section->start + 4, 4);
  if (build_id_size < 8)
    {
      warn (_(".note.gnu.build-id data size is too small\n"));
      return;
    }
  
  if (build_id_size > (section->size - 16))
    {
      warn (_(".note.gnu.build-id data size is too bug\n"));
      return;
    }

  char * filename;
  filename = xmalloc (strlen (".build-id/")
		      + build_id_size * 2 + 2
		      + strlen (".debug")
		      /* The next string should be the same as the longest
			 name found in the prefixes[] array below.  */
		      + strlen ("/usrlib64/debug/usr")
		      + 1);
  void * handle;

  static const char * prefixes[] =
    {
      "",
      ".debug/",
      "/usr/lib/debug/",
      "/usr/lib/debug/usr/",
      "/usr/lib64/debug/",
      "/usr/lib64/debug/usr"
    };
  long unsigned int i;

  for (i = 0; i < ARRAY_SIZE (prefixes); i++)
    {
      handle = try_build_id_prefix (prefixes[i], filename,
				    section->start + 16, build_id_size);
      if (handle != NULL)
	break;
    }
  /* FIXME: TYhe BFD library also tries a global debugfile directory prefix.  */
  if (handle == NULL)
    {
      /* Failed to find a debug file associated with the build-id.
	 This is not an error however, rather it just means that
	 the debug info has probably not been loaded on the system,
	 or that another method is being used to link to the debug
	 info.  */
      free (filename);
      return;
    }

  add_separate_debug_file (filename, handle);
}