static int64_t round_to_kag(int64_t position, int kag_size)
{
    /* TODO: account for run-in? the spec isn't clear whether KAG should account for it */
    /* NOTE: kag_size may be any integer between 1 - 2^10 */
    int64_t ret = (position / kag_size) * kag_size;
    return ret == position ? ret : ret + kag_size;
}
