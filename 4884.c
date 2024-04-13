xcmalloc (size_t nmemb, size_t size)
{
  /* Check for overflow.  */
  if (nmemb >= ~(size_t) 0 / size)
    {
      fprintf (stderr,
	       _("Attempt to allocate an array with an excessive number of elements: 0x%lx\n"),
	       (long) nmemb);
      xexit (1);
    }

  return xmalloc (nmemb * size);
}