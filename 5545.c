save_last_search_pattern(void)
{
    if (++did_save_last_search_spat != 1)
	// nested call, nothing to do
	return;

    saved_last_search_spat = spats[RE_SEARCH];
    if (spats[RE_SEARCH].pat != NULL)
	saved_last_search_spat.pat = vim_strsave(spats[RE_SEARCH].pat);
    saved_last_idx = last_idx;
    saved_no_hlsearch = no_hlsearch;
}