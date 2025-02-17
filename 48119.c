psf_fgets (char *buffer, sf_count_t bufsize, SF_PRIVATE *psf)
{	sf_count_t	k = 0 ;
	sf_count_t		count ;

	while (k < bufsize - 1)
	{	count = read (psf->file.filedes, &(buffer [k]), 1) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0 || buffer [k++] == '\n')
			break ;
		} ;

	buffer [k] = 0 ;

	return k ;
} /* psf_fgets */
