fuzzy_match_item_compare(const void *s1, const void *s2)
{
    int		v1 = ((fuzzyItem_T *)s1)->score;
    int		v2 = ((fuzzyItem_T *)s2)->score;
    int		idx1 = ((fuzzyItem_T *)s1)->idx;
    int		idx2 = ((fuzzyItem_T *)s2)->idx;

    return v1 == v2 ? (idx1 - idx2) : v1 > v2 ? -1 : 1;
}