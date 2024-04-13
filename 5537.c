fuzzy_match_func_compare(const void *s1, const void *s2)
{
    int		v1 = ((fuzmatch_str_T *)s1)->score;
    int		v2 = ((fuzmatch_str_T *)s2)->score;
    int		idx1 = ((fuzmatch_str_T *)s1)->idx;
    int		idx2 = ((fuzmatch_str_T *)s2)->idx;
    char_u	*str1 = ((fuzmatch_str_T *)s1)->str;
    char_u	*str2 = ((fuzmatch_str_T *)s2)->str;

    if (*str1 != '<' && *str2 == '<') return -1;
    if (*str1 == '<' && *str2 != '<') return 1;
    return v1 == v2 ? (idx1 - idx2) : v1 > v2 ? -1 : 1;
}