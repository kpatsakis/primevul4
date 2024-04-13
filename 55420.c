static int user_is_in(const strarray_t *aclbits, const char *user)
{
    int i;
    if (!aclbits) return 0;
    for (i = 0; i+1 < strarray_size(aclbits); i+=2) {
        if (!strcmp(strarray_nth(aclbits, i), user)) return 1;
    }
    return 0;
}
