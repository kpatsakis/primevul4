static int nsv_probe(AVProbeData *p)
{
    int i;
    int score;
    int vsize, asize, auxcount;
    score = 0;
    av_log(NULL, AV_LOG_TRACE, "nsv_probe(), buf_size %d\n", p->buf_size);
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
        if (p->buf[i+0] == 'N' && p->buf[i+1] == 'S' &&
            p->buf[i+2] == 'V' && p->buf[i+3] == 's') {
            score = AVPROBE_SCORE_MAX/5;
            /* Get the chunk size and check if at the end we are getting 0xBEEF */
            auxcount = p->buf[i+19];
            vsize = p->buf[i+20]  | p->buf[i+21] << 8;
            asize = p->buf[i+22]  | p->buf[i+23] << 8;
            vsize = (vsize << 4) | (auxcount >> 4);
            if ((asize + vsize + i + 23) <  p->buf_size - 2) {
                if (p->buf[i+23+asize+vsize+1] == 0xEF &&
                    p->buf[i+23+asize+vsize+2] == 0xBE)
                    return AVPROBE_SCORE_MAX-20;
            }
        }
    }
    /* so we'll have more luck on extension... */
    if (av_match_ext(p->filename, "nsv"))
        return AVPROBE_SCORE_EXTENSION;
    /* FIXME: add mime-type check */
    return score;
}
