flac_write_strings (SF_PRIVATE *psf, FLAC_PRIVATE* pflac)
{	FLAC__StreamMetadata_VorbisComment_Entry entry ;
	int	k, string_count = 0 ;

	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
	{	if (psf->strings.data [k].type != 0)
			string_count ++ ;
		} ;

	if (string_count == 0)
		return ;

	if (pflac->metadata == NULL && (pflac->metadata = FLAC__metadata_object_new (FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL)
	{	psf_log_printf (psf, "FLAC__metadata_object_new returned NULL\n") ;
		return ;
		} ;

	for (k = 0 ; k < SF_MAX_STRINGS && psf->strings.data [k].type != 0 ; k++)
	{	const char * key, * value ;

		switch (psf->strings.data [k].type)
		{	case SF_STR_SOFTWARE :
				key = "software" ;
				break ;
			case SF_STR_TITLE :
				key = "title" ;
				break ;
			case SF_STR_COPYRIGHT :
				key = "copyright" ;
				break ;
			case SF_STR_ARTIST :
				key = "artist" ;
				break ;
			case SF_STR_COMMENT :
				key = "comment" ;
				break ;
			case SF_STR_DATE :
				key = "date" ;
				break ;
			case SF_STR_ALBUM :
				key = "album" ;
				break ;
			case SF_STR_LICENSE :
				key = "license" ;
				break ;
			case SF_STR_TRACKNUMBER :
				key = "tracknumber" ;
				break ;
			case SF_STR_GENRE :
				key = "genre" ;
				break ;
			default :
				continue ;
			} ;

		value = psf->strings.storage + psf->strings.data [k].offset ;

		FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair (&entry, key, value) ;
		FLAC__metadata_object_vorbiscomment_append_comment (pflac->metadata, entry, /* copy */ SF_FALSE) ;
		} ;

	if (! FLAC__stream_encoder_set_metadata (pflac->fse, &pflac->metadata, 1))
	{	printf ("%s %d : fail\n", __func__, __LINE__) ;
		return ;
		} ;

	return ;
} /* flac_write_strings */
