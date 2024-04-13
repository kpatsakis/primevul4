static int get_freq(RangeCoder *rc, unsigned total_freq, unsigned *freq)
{
    if (total_freq == 0)
        return AVERROR_INVALIDDATA;

    rc->range = rc->range / total_freq;

    if (rc->range == 0)
        return AVERROR_INVALIDDATA;

    *freq = rc->code / rc->range;

    return 0;
}
