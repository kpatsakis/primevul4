aiff_write_tailer (SF_PRIVATE *psf)
{	int		k ;

	/* Reset the current header length to zero. */
	psf->header.ptr [0] = 0 ;
	psf->header.indx = 0 ;

	psf->dataend = psf_fseek (psf, 0, SEEK_END) ;

	/* Make sure tailer data starts at even byte offset. Pad if necessary. */
	if (psf->dataend % 2 == 1)
	{	psf_fwrite (psf->header.ptr, 1, 1, psf) ;
		psf->dataend ++ ;
		} ;

	if (psf->peak_info != NULL && psf->peak_info->peak_loc == SF_PEAK_END)
	{	psf_binheader_writef (psf, "Em4", PEAK_MARKER, AIFF_PEAK_CHUNK_SIZE (psf->sf.channels)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "Eft8", (float) psf->peak_info->peaks [k].value, psf->peak_info->peaks [k].position) ;
		} ;

	if (psf->strings.flags & SF_STR_LOCATE_END)
		aiff_write_strings (psf, SF_STR_LOCATE_END) ;

	/* Write the tailer. */
	if (psf->header.indx > 0)
		psf_fwrite (psf->header.ptr, psf->header.indx, 1, psf) ;

	return 0 ;
} /* aiff_write_tailer */
