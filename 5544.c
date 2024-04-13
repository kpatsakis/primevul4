fuzzy_match_func_sort(fuzmatch_str_T *fm, int sz)
{
    // Sort the list by the descending order of the match score
    qsort((void *)fm, (size_t)sz, sizeof(fuzmatch_str_T),
		fuzzy_match_func_compare);
}