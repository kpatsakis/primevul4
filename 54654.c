static av_always_inline int smk_get_code(BitstreamContext *bc, int *recode,
                                         int *last)
{
    register int *table = recode;
    int v;

    while(*table & SMK_NODE) {
        if (bitstream_read_bit(bc))
            table += (*table) & (~SMK_NODE);
        table++;
    }
    v = *table;

    if(v != recode[last[0]]) {
        recode[last[2]] = recode[last[1]];
        recode[last[1]] = recode[last[0]];
        recode[last[0]] = v;
    }
    return v;
}
