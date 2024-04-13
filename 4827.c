display_data (bfd_size_type          printed,
	      const unsigned char *  data,
	      const bfd_size_type    len)
{
  if (do_wide || len < ((80 - printed) / 3))
    for (printed = 0; printed < len; ++printed)
      printf (" %02x", data[printed]);
  else
    {
      for (printed = 0; printed < len; ++printed)
	{
	  if (printed % (80 / 3) == 0)
	    putchar ('\n');
	  printf (" %02x", data[printed]);
	}
    }
}