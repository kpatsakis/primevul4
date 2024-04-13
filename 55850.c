rm_ac3_swap_bytes (AVStream *st, AVPacket *pkt)
{
    uint8_t *ptr;
    int j;

    if (st->codecpar->codec_id == AV_CODEC_ID_AC3) {
        ptr = pkt->data;
        for (j=0;j<pkt->size;j+=2) {
            FFSWAP(int, ptr[0], ptr[1]);
            ptr += 2;
        }
    }
}
