static int nsv_probe(AVProbeData *p)
{
    int i, score = 0;

    /* check file header */
    /* streamed files might not have any header */
    if (p->buf[0] == 'N' && p->buf[1] == 'S' &&
        p->buf[2] == 'V' && (p->buf[3] == 'f' || p->buf[3] == 's'))
        return AVPROBE_SCORE_MAX;
    /* XXX: do streamed files always start at chunk boundary ?? */
    /* or do we need to search NSVs in the byte stream ? */
    /* seems the servers don't bother starting clean chunks... */
    /* sometimes even the first header is at 9KB or something :^) */
    for (i = 1; i < p->buf_size - 3; i++) {
        if (AV_RL32(p->buf + i) == AV_RL32("NSVs")) {
            /* Get the chunk size and check if at the end we are getting 0xBEEF */
            int vsize = AV_RL24(p->buf+i+19) >> 4;
            int asize = AV_RL16(p->buf+i+22);
            int offset = i + 23 + asize + vsize + 1;
            if (offset <= p->buf_size - 2 && AV_RL16(p->buf + offset) == 0xBEEF)
                return 4*AVPROBE_SCORE_MAX/5;
            score = AVPROBE_SCORE_MAX/5;
        }
    }
    /* so we'll have more luck on extension... */
    if (av_match_ext(p->filename, "nsv"))
        return AVPROBE_SCORE_EXTENSION;
    /* FIXME: add mime-type check */
    return score;
}
