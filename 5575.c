fuzmatch_str_free(fuzmatch_str_T *fuzmatch, int count)
{
    int i;

    if (fuzmatch == NULL)
	return;
    for (i = 0; i < count; ++i)
	vim_free(fuzmatch[i].str);
    vim_free(fuzmatch);
}