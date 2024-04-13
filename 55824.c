static int rl2_probe(AVProbeData *p)
{

    if(AV_RB32(&p->buf[0]) != FORM_TAG)
        return 0;

    if(AV_RB32(&p->buf[8]) != RLV2_TAG &&
        AV_RB32(&p->buf[8]) != RLV3_TAG)
        return 0;

    return AVPROBE_SCORE_MAX;
}
