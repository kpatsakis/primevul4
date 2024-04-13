static int cine_read_probe(AVProbeData *p)
{
    int HeaderSize;
    if (p->buf[0] == 'C' && p->buf[1] == 'I' &&  // Type
        (HeaderSize = AV_RL16(p->buf + 2)) >= 0x2C &&  // HeaderSize
        AV_RL16(p->buf + 4) <= CC_UNINT &&       // Compression
        AV_RL16(p->buf + 6) <= 1 &&              // Version
        AV_RL32(p->buf + 20) &&                  // ImageCount
        AV_RL32(p->buf + 24) >= HeaderSize &&    // OffImageHeader
        AV_RL32(p->buf + 28) >= HeaderSize &&    // OffSetup
        AV_RL32(p->buf + 32) >= HeaderSize)      // OffImageOffsets
        return AVPROBE_SCORE_MAX;
    return 0;
}
