restore_search_patterns(void)
{
    if (--save_level == 0)
    {
	vim_free(spats[0].pat);
	spats[0] = saved_spats[0];
#if defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
	vim_free(spats[1].pat);
	spats[1] = saved_spats[1];
	vim_free(mr_pattern);
	mr_pattern = saved_mr_pattern;
#ifdef FEAT_SEARCH_EXTRA
	last_idx = saved_spats_last_idx;
	set_no_hlsearch(saved_spats_no_hlsearch);
#endif
    }
}