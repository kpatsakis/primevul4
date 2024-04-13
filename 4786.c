load_cu_tu_indexes (void *file)
{
  /* If we have already loaded (or tried to load) the CU and TU indexes
     then do not bother to repeat the task.  */
  if (cu_tu_indexes_read == -1)
    {
      cu_tu_indexes_read = true;

      if (load_debug_section_with_follow (dwp_cu_index, file))
	if (! process_cu_tu_index (&debug_displays [dwp_cu_index].section, 0))
	  cu_tu_indexes_read = false;

      if (load_debug_section_with_follow (dwp_tu_index, file))
	if (! process_cu_tu_index (&debug_displays [dwp_tu_index].section, 0))
	  cu_tu_indexes_read = false;
    }

  return (bool) cu_tu_indexes_read;
}