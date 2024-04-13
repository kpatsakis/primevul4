marker_to_position (const MARK_ID_POS *m, uint16_t n, int marksize)
{	int i ;

	for (i = 0 ; i < marksize ; i++)
		if (m [i].markerID == n)
			return m [i].position ;
	return 0 ;
} /* marker_to_position */
