psf_fread (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t count ;
	DWORD dwNumberOfBytesRead ;

	if (psf->virtual_io)
		return psf->vio.read (ptr, bytes*items, psf->vio_user_data) / bytes ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : (ssize_t) items ;

		if (ReadFile (psf->file.handle, ((char*) ptr) + total, count, &dwNumberOfBytesRead, 0) == 0)
		{	psf_log_syserr (psf, GetLastError ()) ;
			break ;
			}
		else
			count = dwNumberOfBytesRead ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	if (psf->is_pipe)
		psf->pipeoffset += total ;

	return total / bytes ;
} /* psf_fread */
