psf_open_fd (PSF_FILE * pfile)
{	int fd, oflag, mode ;

	/*
	** Sanity check. If everything is OK, this test and the printfs will
	** be optimised out. This is meant to catch the problems caused by
	** "sfconfig.h" being included after <stdio.h>.
	*/
	if (sizeof (sf_count_t) != 8)
	{	puts ("\n\n*** Fatal error : sizeof (sf_count_t) != 8") ;
		puts ("*** This means that libsndfile was not configured correctly.\n") ;
		exit (1) ;
		} ;

	switch (pfile->mode)
	{	case SFM_READ :
				oflag = O_RDONLY | O_BINARY ;
				mode = 0 ;
				break ;

		case SFM_WRITE :
				oflag = O_WRONLY | O_CREAT | O_TRUNC | O_BINARY ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ;
				break ;

		case SFM_RDWR :
				oflag = O_RDWR | O_CREAT | O_BINARY ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ;
				break ;

		default :
				return - SFE_BAD_OPEN_MODE ;
				break ;
		} ;

	if (mode == 0)
		fd = open (pfile->path.c, oflag) ;
	else
		fd = open (pfile->path.c, oflag, mode) ;

	return fd ;
} /* psf_open_fd */
