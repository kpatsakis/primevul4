psf_open_rsrc (SF_PRIVATE *psf)
{
	if (psf->rsrc.handle != NULL)
		return 0 ;

	/* Test for MacOSX style resource fork on HPFS or HPFS+ filesystems. */
	snprintf (psf->rsrc.path.c, sizeof (psf->rsrc.path.c), "%s/rsrc", psf->file.path.c) ;
	psf->error = SFE_NO_ERROR ;
	if ((psf->rsrc.handle = psf_open_handle (&psf->rsrc)) != NULL)
	{	psf->rsrclength = psf_get_filelen_handle (psf->rsrc.handle) ;
		return SFE_NO_ERROR ;
		} ;

	/*
	** Now try for a resource fork stored as a separate file in the same
	** directory, but preceded with a dot underscore.
	*/
	snprintf (psf->rsrc.path.c, sizeof (psf->rsrc.path.c), "%s._%s", psf->file.dir.c, psf->file.name.c) ;
	psf->error = SFE_NO_ERROR ;
	if ((psf->rsrc.handle = psf_open_handle (&psf->rsrc)) != NULL)
	{	psf->rsrclength = psf_get_filelen_handle (psf->rsrc.handle) ;
		return SFE_NO_ERROR ;
		} ;

	/*
	** Now try for a resource fork stored in a separate file in the
	** .AppleDouble/ directory.
	*/
	snprintf (psf->rsrc.path.c, sizeof (psf->rsrc.path.c), "%s.AppleDouble/%s", psf->file.dir.c, psf->file.name.c) ;
	psf->error = SFE_NO_ERROR ;
	if ((psf->rsrc.handle = psf_open_handle (&psf->rsrc)) != NULL)
	{	psf->rsrclength = psf_get_filelen_handle (psf->rsrc.handle) ;
		return SFE_NO_ERROR ;
		} ;

	/* No resource file found. */
	if (psf->rsrc.handle == NULL)
		psf_log_syserr (psf, GetLastError ()) ;

	psf->rsrc.handle = NULL ;

	return psf->error ;
} /* psf_open_rsrc */
