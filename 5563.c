reset_search_dir(void)
{
    spats[0].off.dir = '/';
#if defined(FEAT_EVAL)
    set_vv_searchforward();
#endif
}