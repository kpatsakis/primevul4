aiff_write_strings (SF_PRIVATE *psf, int location)
{	int	k, slen ;

	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
	{	if (psf->strings.data [k].type == 0)
			break ;

		if (psf->strings.data [k].flags != location)
			continue ;

		switch (psf->strings.data [k].type)
		{	case SF_STR_SOFTWARE :
				slen = strlen (psf->strings.storage + psf->strings.data [k].offset) ;
				psf_binheader_writef (psf, "Em4mb", APPL_MARKER, slen + 4, m3ga_MARKER, psf->strings.storage + psf->strings.data [k].offset, make_size_t (slen + (slen & 1))) ;
				break ;

			case SF_STR_TITLE :
				psf_binheader_writef (psf, "EmS", NAME_MARKER, psf->strings.storage + psf->strings.data [k].offset) ;
				break ;

			case SF_STR_COPYRIGHT :
				psf_binheader_writef (psf, "EmS", c_MARKER, psf->strings.storage + psf->strings.data [k].offset) ;
				break ;

			case SF_STR_ARTIST :
				psf_binheader_writef (psf, "EmS", AUTH_MARKER, psf->strings.storage + psf->strings.data [k].offset) ;
				break ;

			case SF_STR_COMMENT :
				psf_binheader_writef (psf, "EmS", ANNO_MARKER, psf->strings.storage + psf->strings.data [k].offset) ;
				break ;

			/*
			case SF_STR_DATE :
				psf_binheader_writef (psf, "Ems", ICRD_MARKER, psf->strings.data [k].str) ;
				break ;
			*/
			} ;
		} ;

	return ;
} /* aiff_write_strings */
