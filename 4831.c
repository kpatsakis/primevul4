xcalloc2 (size_t nmemb, size_t size)
{
  /* Check for overflow.  */
  if (nmemb >= ~(size_t) 0 / size)
    {
      error (_("Attempt to allocate a zero'ed array with an excessive number of elements: 0x%lx\n"),
	     (long) nmemb);
      xexit (1);
    }

  return xcalloc (nmemb, size);
}