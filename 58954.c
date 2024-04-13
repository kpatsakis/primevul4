aiff_command (SF_PRIVATE * psf, int command, void * UNUSED (data), int UNUSED (datasize))
{	AIFF_PRIVATE	*paiff ;

	if ((paiff = psf->container_data) == NULL)
		return SFE_INTERNAL ;

	switch (command)
	{	case SFC_SET_CHANNEL_MAP_INFO :
			paiff->chanmap_tag = aiff_caf_find_channel_layout_tag (psf->channel_map, psf->sf.channels) ;
			return (paiff->chanmap_tag != 0) ;

		default :
			break ;
	} ;

	return 0 ;
} /* aiff_command */
