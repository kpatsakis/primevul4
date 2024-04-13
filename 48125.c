psf_fopen (SF_PRIVATE *psf, const char *pathname, int open_mode)
{	int oflag, mode ;

	switch (open_mode)
	{	case SFM_READ :
				oflag = O_RDONLY | O_BINARY ;
				mode = 0 ;
				break ;

		case SFM_WRITE :
				oflag = O_WRONLY | O_CREAT | O_TRUNC | O_BINARY ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP ;
				break ;

		case SFM_RDWR :
				oflag = O_RDWR | O_CREAT | O_BINARY ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP ;
				break ;

		default :
				psf->error = SFE_BAD_OPEN_MODE ;
				return -1 ;
				break ;
		} ;

	if (mode == 0)
		psf->file.filedes = open (pathname, oflag) ;
	else
		psf->file.filedes = open (pathname, oflag, mode) ;

	if (psf->file.filedes == -1)
		psf_log_syserr (psf, errno) ;

	return psf->file.filedes ;
} /* psf_fopen */
