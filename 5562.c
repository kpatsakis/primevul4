fuzzymatches_to_strmatches(
	fuzmatch_str_T	*fuzmatch,
	char_u		***matches,
	int		count,
	int		funcsort)
{
    int		i;

    if (count <= 0)
	return OK;

    *matches = ALLOC_MULT(char_u *, count);
    if (*matches == NULL)
    {
	fuzmatch_str_free(fuzmatch, count);
	return FAIL;
    }

    // Sort the list by the descending order of the match score
    if (funcsort)
	fuzzy_match_func_sort((void *)fuzmatch, (size_t)count);
    else
	fuzzy_match_str_sort((void *)fuzmatch, (size_t)count);

    for (i = 0; i < count; i++)
	(*matches)[i] = fuzmatch[i].str;
    vim_free(fuzmatch);

    return OK;
}