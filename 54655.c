static int smka_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_frame_ptr, AVPacket *avpkt)
{
    AVFrame *frame     = data;
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    BitstreamContext bc;
    HuffContext h[4] = { { 0 } };
    VLC vlc[4]       = { { 0 } };
    int16_t *samples;
    uint8_t *samples8;
    int val;
    int i, res, ret;
    int unp_size;
    int bits, stereo;
    int pred[2] = {0, 0};

    if (buf_size <= 4) {
        av_log(avctx, AV_LOG_ERROR, "packet is too small\n");
        return AVERROR_INVALIDDATA;
    }

    unp_size = AV_RL32(buf);

    bitstream_init8(&bc, buf + 4, buf_size - 4);

    if (!bitstream_read_bit(&bc)) {
        av_log(avctx, AV_LOG_INFO, "Sound: no data\n");
        *got_frame_ptr = 0;
        return 1;
    }
    stereo = bitstream_read_bit(&bc);
    bits   = bitstream_read_bit(&bc);
    if (stereo ^ (avctx->channels != 1)) {
        av_log(avctx, AV_LOG_ERROR, "channels mismatch\n");
        return AVERROR_INVALIDDATA;
    }
    if (bits && avctx->sample_fmt == AV_SAMPLE_FMT_U8) {
        av_log(avctx, AV_LOG_ERROR, "sample format mismatch\n");
        return AVERROR_INVALIDDATA;
    }
    if (unp_size % (avctx->channels * (bits + 1))) {
        av_log(avctx, AV_LOG_ERROR,
               "The buffer does not contain an integer number of samples\n");
        return AVERROR_INVALIDDATA;
    }

    /* get output buffer */
    frame->nb_samples = unp_size / (avctx->channels * (bits + 1));
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }
    samples  = (int16_t *)frame->data[0];
    samples8 =            frame->data[0];

    for(i = 0; i < (1 << (bits + stereo)); i++) {
        h[i].length = 256;
        h[i].maxlength = 0;
        h[i].current = 0;
        h[i].bits = av_mallocz(256 * 4);
        h[i].lengths = av_mallocz(256 * sizeof(int));
        h[i].values = av_mallocz(256 * sizeof(int));
        if (!h[i].bits || !h[i].lengths || !h[i].values) {
            ret = AVERROR(ENOMEM);
            goto error;
        }
        bitstream_skip(&bc, 1);
        if (smacker_decode_tree(&bc, &h[i], 0, 0) < 0) {
            ret = AVERROR_INVALIDDATA;
            goto error;
        }
        bitstream_skip(&bc, 1);
        if(h[i].current > 1) {
            res = init_vlc(&vlc[i], SMKTREE_BITS, h[i].length,
                    h[i].lengths, sizeof(int), sizeof(int),
                    h[i].bits, sizeof(uint32_t), sizeof(uint32_t), INIT_VLC_LE);
            if(res < 0) {
                av_log(avctx, AV_LOG_ERROR, "Cannot build VLC table\n");
                ret = AVERROR_INVALIDDATA;
                goto error;
            }
        }
    }
    /* this codec relies on wraparound instead of clipping audio */
    if(bits) { //decode 16-bit data
        for(i = stereo; i >= 0; i--)
            pred[i] = sign_extend(av_bswap16(bitstream_read(&bc, 16)), 16);
        for(i = 0; i <= stereo; i++)
            *samples++ = pred[i];
        for(; i < unp_size / 2; i++) {
            if(i & stereo) {
                if(vlc[2].table)
                    res = bitstream_read_vlc(&bc, vlc[2].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val  = h[2].values[res];
                if(vlc[3].table)
                    res = bitstream_read_vlc(&bc, vlc[3].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val |= h[3].values[res] << 8;
                pred[1] += sign_extend(val, 16);
                *samples++ = pred[1];
            } else {
                if(vlc[0].table)
                    res = bitstream_read_vlc(&bc, vlc[0].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val  = h[0].values[res];
                if(vlc[1].table)
                    res = bitstream_read_vlc(&bc, vlc[1].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                val |= h[1].values[res] << 8;
                pred[0] += sign_extend(val, 16);
                *samples++ = pred[0];
            }
        }
    } else { //8-bit data
        for(i = stereo; i >= 0; i--)
            pred[i] = bitstream_read(&bc, 8);
        for(i = 0; i <= stereo; i++)
            *samples8++ = pred[i];
        for(; i < unp_size; i++) {
            if(i & stereo){
                if(vlc[1].table)
                    res = bitstream_read_vlc(&bc, vlc[1].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                pred[1] += sign_extend(h[1].values[res], 8);
                *samples8++ = pred[1];
            } else {
                if(vlc[0].table)
                    res = bitstream_read_vlc(&bc, vlc[0].table, SMKTREE_BITS, 3);
                else
                    res = 0;
                pred[0] += sign_extend(h[0].values[res], 8);
                *samples8++ = pred[0];
            }
        }
    }

    *got_frame_ptr = 1;
    ret = buf_size;

error:
    for(i = 0; i < 4; i++) {
        if(vlc[i].table)
            ff_free_vlc(&vlc[i]);
        av_free(h[i].bits);
        av_free(h[i].lengths);
        av_free(h[i].values);
    }

    return ret;
}
