static int cenc_filter(MOVContext *c, MOVStreamContext *sc, int64_t index, uint8_t *input, int size)
{
    uint32_t encrypted_bytes;
    uint16_t subsample_count;
    uint16_t clear_bytes;
    uint8_t* input_end = input + size;
    int ret;

    if (index != sc->cenc.auxiliary_info_index) {
        ret = mov_seek_auxiliary_info(c, sc, index);
        if (ret < 0) {
            return ret;
        }
    }

    /* read the iv */
    if (AES_CTR_IV_SIZE > sc->cenc.auxiliary_info_end - sc->cenc.auxiliary_info_pos) {
        av_log(c->fc, AV_LOG_ERROR, "failed to read iv from the auxiliary info\n");
        return AVERROR_INVALIDDATA;
    }

    av_aes_ctr_set_iv(sc->cenc.aes_ctr, sc->cenc.auxiliary_info_pos);
    sc->cenc.auxiliary_info_pos += AES_CTR_IV_SIZE;

    if (!sc->cenc.use_subsamples)
    {
        /* decrypt the whole packet */
        av_aes_ctr_crypt(sc->cenc.aes_ctr, input, input, size);
        return 0;
    }

    /* read the subsample count */
    if (sizeof(uint16_t) > sc->cenc.auxiliary_info_end - sc->cenc.auxiliary_info_pos) {
        av_log(c->fc, AV_LOG_ERROR, "failed to read subsample count from the auxiliary info\n");
        return AVERROR_INVALIDDATA;
    }

    subsample_count = AV_RB16(sc->cenc.auxiliary_info_pos);
    sc->cenc.auxiliary_info_pos += sizeof(uint16_t);

    for (; subsample_count > 0; subsample_count--)
    {
        if (6 > sc->cenc.auxiliary_info_end - sc->cenc.auxiliary_info_pos) {
            av_log(c->fc, AV_LOG_ERROR, "failed to read subsample from the auxiliary info\n");
            return AVERROR_INVALIDDATA;
        }

        /* read the number of clear / encrypted bytes */
        clear_bytes = AV_RB16(sc->cenc.auxiliary_info_pos);
        sc->cenc.auxiliary_info_pos += sizeof(uint16_t);
        encrypted_bytes = AV_RB32(sc->cenc.auxiliary_info_pos);
        sc->cenc.auxiliary_info_pos += sizeof(uint32_t);

        if ((uint64_t)clear_bytes + encrypted_bytes > input_end - input) {
            av_log(c->fc, AV_LOG_ERROR, "subsample size exceeds the packet size left\n");
            return AVERROR_INVALIDDATA;
        }

        /* skip the clear bytes */
        input += clear_bytes;

        /* decrypt the encrypted bytes */
        av_aes_ctr_crypt(sc->cenc.aes_ctr, input, input, encrypted_bytes);
        input += encrypted_bytes;
    }

    if (input < input_end) {
        av_log(c->fc, AV_LOG_ERROR, "leftover packet bytes after subsample processing\n");
        return AVERROR_INVALIDDATA;
    }

    sc->cenc.auxiliary_info_index++;
    return 0;
}
