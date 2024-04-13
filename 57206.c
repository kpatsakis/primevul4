static int avi_probe(AVProbeData *p)
{
    int i;

    /* check file header */
    for (i = 0; avi_headers[i][0]; i++)
        if (AV_RL32(p->buf    ) == AV_RL32(avi_headers[i]    ) &&
            AV_RL32(p->buf + 8) == AV_RL32(avi_headers[i] + 4))
            return AVPROBE_SCORE_MAX;

    return 0;
}
