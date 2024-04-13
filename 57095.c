static int get_freq0(RangeCoder *rc, unsigned total_freq, unsigned *freq)
{
    if (rc->range == 0)
        return AVERROR_INVALIDDATA;

    *freq = total_freq * (uint64_t)(rc->code - rc->code1) / rc->range;

    return 0;
}
