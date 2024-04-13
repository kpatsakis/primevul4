static int mv_probe(AVProbeData *p)
{
    if (AV_RB32(p->buf) == MKBETAG('M', 'O', 'V', 'I') &&
        AV_RB16(p->buf + 4) < 3)
        return AVPROBE_SCORE_MAX;
    return 0;
}
