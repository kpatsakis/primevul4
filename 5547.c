set_last_search_pat(
    char_u	*s,
    int		idx,
    int		magic,
    int		setlast)
{
    vim_free(spats[idx].pat);
    // An empty string means that nothing should be matched.
    if (*s == NUL)
	spats[idx].pat = NULL;
    else
	spats[idx].pat = vim_strsave(s);
    spats[idx].magic = magic;
    spats[idx].no_scs = FALSE;
    spats[idx].off.dir = '/';
#if defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
    spats[idx].off.line = FALSE;
    spats[idx].off.end = FALSE;
    spats[idx].off.off = 0;
    if (setlast)
	last_idx = idx;
    if (save_level)
    {
	vim_free(saved_spats[idx].pat);
	saved_spats[idx] = spats[0];
	if (spats[idx].pat == NULL)
	    saved_spats[idx].pat = NULL;
	else
	    saved_spats[idx].pat = vim_strsave(spats[idx].pat);
# ifdef FEAT_SEARCH_EXTRA
	saved_spats_last_idx = last_idx;
# endif
    }
# ifdef FEAT_SEARCH_EXTRA
    // If 'hlsearch' set and search pat changed: need redraw.
    if (p_hls && idx == last_idx && !no_hlsearch)
	redraw_all_later(SOME_VALID);
# endif
}