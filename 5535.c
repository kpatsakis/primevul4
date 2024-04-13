free_search_patterns(void)
{
    vim_free(spats[0].pat);
    vim_free(spats[1].pat);
    VIM_CLEAR(mr_pattern);
}