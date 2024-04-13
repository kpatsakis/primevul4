static const VP56mv *get_bmv_ptr(const VP8Macroblock *mb, int subblock)
{
    return &mb->bmv[mb->mode == VP8_MVMODE_SPLIT ? vp8_mbsplits[mb->partitioning][subblock] : 0];
}
