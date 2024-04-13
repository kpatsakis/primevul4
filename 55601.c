static int nsv_resync(AVFormatContext *s)
{
    NSVContext *nsv = s->priv_data;
    AVIOContext *pb = s->pb;
    uint32_t v = 0;
    int i;

    for (i = 0; i < NSV_MAX_RESYNC; i++) {
        if (avio_feof(pb)) {
            av_log(s, AV_LOG_TRACE, "NSV EOF\n");
            nsv->state = NSV_UNSYNC;
            return -1;
        }
        v <<= 8;
        v |= avio_r8(pb);
        if (i < 8) {
            av_log(s, AV_LOG_TRACE, "NSV resync: [%d] = %02"PRIx32"\n", i, v & 0x0FF);
        }

        if ((v & 0x0000ffff) == 0xefbe) { /* BEEF */
            av_log(s, AV_LOG_TRACE, "NSV resynced on BEEF after %d bytes\n", i+1);
            nsv->state = NSV_FOUND_BEEF;
            return 0;
        }
        /* we read as big-endian, thus the MK*BE* */
        if (v == TB_NSVF) { /* NSVf */
            av_log(s, AV_LOG_TRACE, "NSV resynced on NSVf after %d bytes\n", i+1);
            nsv->state = NSV_FOUND_NSVF;
            return 0;
        }
        if (v == MKBETAG('N', 'S', 'V', 's')) { /* NSVs */
            av_log(s, AV_LOG_TRACE, "NSV resynced on NSVs after %d bytes\n", i+1);
            nsv->state = NSV_FOUND_NSVS;
            return 0;
        }

    }
    av_log(s, AV_LOG_TRACE, "NSV sync lost\n");
    return -1;
}
