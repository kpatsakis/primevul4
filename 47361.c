int ff_h264_decode_extradata(H264Context *h, const uint8_t *buf, int size)
{
    AVCodecContext *avctx = h->avctx;
    int ret;

    if (!buf || size <= 0)
        return -1;

    if (buf[0] == 1) {
        int i, cnt, nalsize;
        const unsigned char *p = buf;

        h->is_avc = 1;

        if (size < 7) {
            av_log(avctx, AV_LOG_ERROR,
                   "avcC %d too short\n", size);
            return AVERROR_INVALIDDATA;
        }
        /* sps and pps in the avcC always have length coded with 2 bytes,
         * so put a fake nal_length_size = 2 while parsing them */
        h->nal_length_size = 2;
        cnt = *(p + 5) & 0x1f; // Number of sps
        p  += 6;
        for (i = 0; i < cnt; i++) {
            nalsize = AV_RB16(p) + 2;
            if(nalsize > size - (p-buf))
                return AVERROR_INVALIDDATA;
            ret = decode_nal_units(h, p, nalsize, 1);
            if (ret < 0) {
                av_log(avctx, AV_LOG_ERROR,
                       "Decoding sps %d from avcC failed\n", i);
                return ret;
            }
            p += nalsize;
        }
        cnt = *(p++); // Number of pps
        for (i = 0; i < cnt; i++) {
            nalsize = AV_RB16(p) + 2;
            if(nalsize > size - (p-buf))
                return AVERROR_INVALIDDATA;
            ret = decode_nal_units(h, p, nalsize, 1);
            if (ret < 0) {
                av_log(avctx, AV_LOG_ERROR,
                       "Decoding pps %d from avcC failed\n", i);
                return ret;
            }
            p += nalsize;
        }
        h->nal_length_size = (buf[4] & 0x03) + 1;
    } else {
        h->is_avc = 0;
        ret = decode_nal_units(h, buf, size, 1);
        if (ret < 0)
            return ret;
    }
    return size;
}
