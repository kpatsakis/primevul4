init_mime( void )
    {
    int i;

    /* Sort the tables so we can do binary search. */
    qsort( enc_tab, n_enc_tab, sizeof(*enc_tab), ext_compare );
    qsort( typ_tab, n_typ_tab, sizeof(*typ_tab), ext_compare );

    /* Fill in the lengths. */
    for ( i = 0; i < n_enc_tab; ++i )
	{
	enc_tab[i].ext_len = strlen( enc_tab[i].ext );
	enc_tab[i].val_len = strlen( enc_tab[i].val );
	}
    for ( i = 0; i < n_typ_tab; ++i )
	{
	typ_tab[i].ext_len = strlen( typ_tab[i].ext );
	typ_tab[i].val_len = strlen( typ_tab[i].val );
	}

    }
