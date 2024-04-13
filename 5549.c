save_search_patterns(void)
{
    if (save_level++ == 0)
    {
	saved_spats[0] = spats[0];
	if (spats[0].pat != NULL)
	    saved_spats[0].pat = vim_strsave(spats[0].pat);
	saved_spats[1] = spats[1];
	if (spats[1].pat != NULL)
	    saved_spats[1].pat = vim_strsave(spats[1].pat);
	if (mr_pattern == NULL)
	    saved_mr_pattern = NULL;
	else
	    saved_mr_pattern = vim_strsave(mr_pattern);
#ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
	saved_spats_no_hlsearch = no_hlsearch;
#endif
    }
}