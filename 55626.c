static int mxf_match_uid(const UID key, const UID uid, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        if (i != 7 && key[i] != uid[i])
            return 0;
    }
    return 1;
}
