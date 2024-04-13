aiff_rewrite_header (SF_PRIVATE *psf)
{
	/* Assuming here that the header has already been written and just
	** needs to be corrected for new data length. That means that we
	** only change the length fields of the FORM and SSND chunks ;
	** everything else can be skipped over.
	*/
	int k, ch, comm_size, comm_frames ;

	psf_fseek (psf, 0, SEEK_SET) ;
	psf_fread (psf->header.ptr, psf->dataoffset, 1, psf) ;

	psf->header.indx = 0 ;

	/* FORM chunk. */
	psf_binheader_writef (psf, "Etm8", FORM_MARKER, psf->filelength - 8) ;

	/* COMM chunk. */
	if ((k = psf_find_read_chunk_m32 (&psf->rchunks, COMM_MARKER)) >= 0)
	{	psf->header.indx = psf->rchunks.chunks [k].offset - 8 ;
		comm_frames = psf->sf.frames ;
		comm_size = psf->rchunks.chunks [k].len ;
		psf_binheader_writef (psf, "Em42t4", COMM_MARKER, comm_size, psf->sf.channels, comm_frames) ;
		} ;

	/* PEAK chunk. */
	if ((k = psf_find_read_chunk_m32 (&psf->rchunks, PEAK_MARKER)) >= 0)
	{	psf->header.indx = psf->rchunks.chunks [k].offset - 8 ;
		psf_binheader_writef (psf, "Em4", PEAK_MARKER, AIFF_PEAK_CHUNK_SIZE (psf->sf.channels)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (ch = 0 ; ch < psf->sf.channels ; ch++)
			psf_binheader_writef (psf, "Eft8", (float) psf->peak_info->peaks [ch].value, psf->peak_info->peaks [ch].position) ;
		} ;


	/* SSND chunk. */
	if ((k = psf_find_read_chunk_m32 (&psf->rchunks, SSND_MARKER)) >= 0)
	{	psf->header.indx = psf->rchunks.chunks [k].offset - 8 ;
		psf_binheader_writef (psf, "Etm8", SSND_MARKER, psf->datalength + SIZEOF_SSND_CHUNK) ;
		} ;

	/* Header mangling complete so write it out. */
	psf_fseek (psf, 0, SEEK_SET) ;
	psf_fwrite (psf->header.ptr, psf->header.indx, 1, psf) ;

	return ;
} /* aiff_rewrite_header */
