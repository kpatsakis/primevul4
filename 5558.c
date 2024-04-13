save_re_pat(int idx, char_u *pat, int magic)
{
    if (spats[idx].pat != pat)
    {
	vim_free(spats[idx].pat);
	spats[idx].pat = vim_strsave(pat);
	spats[idx].magic = magic;
	spats[idx].no_scs = no_smartcase;
	last_idx = idx;
#ifdef FEAT_SEARCH_EXTRA
	// If 'hlsearch' set and search pat changed: need redraw.
	if (p_hls)
	    redraw_all_later(SOME_VALID);
	set_no_hlsearch(FALSE);
#endif
    }
}