static int get_audio_frame_duration(enum AVCodecID id, int sr, int ch, int ba,
                                    uint32_t tag, int bits_per_coded_sample, int64_t bitrate,
                                    uint8_t * extradata, int frame_size, int frame_bytes)
{
    int bps = av_get_exact_bits_per_sample(id);
    int framecount = (ba > 0 && frame_bytes / ba > 0) ? frame_bytes / ba : 1;

    /* codecs with an exact constant bits per sample */
    if (bps > 0 && ch > 0 && frame_bytes > 0 && ch < 32768 && bps < 32768)
        return (frame_bytes * 8LL) / (bps * ch);
    bps = bits_per_coded_sample;

    /* codecs with a fixed packet duration */
    switch (id) {
    case AV_CODEC_ID_ADPCM_ADX:    return   32;
    case AV_CODEC_ID_ADPCM_IMA_QT: return   64;
    case AV_CODEC_ID_ADPCM_EA_XAS: return  128;
    case AV_CODEC_ID_AMR_NB:
    case AV_CODEC_ID_EVRC:
    case AV_CODEC_ID_GSM:
    case AV_CODEC_ID_QCELP:
    case AV_CODEC_ID_RA_288:       return  160;
    case AV_CODEC_ID_AMR_WB:
    case AV_CODEC_ID_GSM_MS:       return  320;
    case AV_CODEC_ID_MP1:          return  384;
    case AV_CODEC_ID_ATRAC1:       return  512;
    case AV_CODEC_ID_ATRAC3:       return 1024 * framecount;
    case AV_CODEC_ID_ATRAC3P:      return 2048;
    case AV_CODEC_ID_MP2:
    case AV_CODEC_ID_MUSEPACK7:    return 1152;
    case AV_CODEC_ID_AC3:          return 1536;
    }

    if (sr > 0) {
        /* calc from sample rate */
        if (id == AV_CODEC_ID_TTA)
            return 256 * sr / 245;
        else if (id == AV_CODEC_ID_DST)
            return 588 * sr / 44100;

        if (ch > 0) {
            /* calc from sample rate and channels */
            if (id == AV_CODEC_ID_BINKAUDIO_DCT)
                return (480 << (sr / 22050)) / ch;
        }
    }

    if (ba > 0) {
        /* calc from block_align */
        if (id == AV_CODEC_ID_SIPR) {
            switch (ba) {
            case 20: return 160;
            case 19: return 144;
            case 29: return 288;
            case 37: return 480;
            }
        } else if (id == AV_CODEC_ID_ILBC) {
            switch (ba) {
            case 38: return 160;
            case 50: return 240;
            }
        }
    }

    if (frame_bytes > 0) {
        /* calc from frame_bytes only */
        if (id == AV_CODEC_ID_TRUESPEECH)
            return 240 * (frame_bytes / 32);
        if (id == AV_CODEC_ID_NELLYMOSER)
            return 256 * (frame_bytes / 64);
        if (id == AV_CODEC_ID_RA_144)
            return 160 * (frame_bytes / 20);
        if (id == AV_CODEC_ID_G723_1)
            return 240 * (frame_bytes / 24);

        if (bps > 0) {
            /* calc from frame_bytes and bits_per_coded_sample */
            if (id == AV_CODEC_ID_ADPCM_G726)
                return frame_bytes * 8 / bps;
        }

        if (ch > 0 && ch < INT_MAX/16) {
            /* calc from frame_bytes and channels */
            switch (id) {
            case AV_CODEC_ID_ADPCM_AFC:
                return frame_bytes / (9 * ch) * 16;
            case AV_CODEC_ID_ADPCM_PSX:
            case AV_CODEC_ID_ADPCM_DTK:
                return frame_bytes / (16 * ch) * 28;
            case AV_CODEC_ID_ADPCM_4XM:
            case AV_CODEC_ID_ADPCM_IMA_DAT4:
            case AV_CODEC_ID_ADPCM_IMA_ISS:
                return (frame_bytes - 4 * ch) * 2 / ch;
            case AV_CODEC_ID_ADPCM_IMA_SMJPEG:
                return (frame_bytes - 4) * 2 / ch;
            case AV_CODEC_ID_ADPCM_IMA_AMV:
                return (frame_bytes - 8) * 2 / ch;
            case AV_CODEC_ID_ADPCM_THP:
            case AV_CODEC_ID_ADPCM_THP_LE:
                if (extradata)
                    return frame_bytes * 14 / (8 * ch);
                break;
            case AV_CODEC_ID_ADPCM_XA:
                return (frame_bytes / 128) * 224 / ch;
            case AV_CODEC_ID_INTERPLAY_DPCM:
                return (frame_bytes - 6 - ch) / ch;
            case AV_CODEC_ID_ROQ_DPCM:
                return (frame_bytes - 8) / ch;
            case AV_CODEC_ID_XAN_DPCM:
                return (frame_bytes - 2 * ch) / ch;
            case AV_CODEC_ID_MACE3:
                return 3 * frame_bytes / ch;
            case AV_CODEC_ID_MACE6:
                return 6 * frame_bytes / ch;
            case AV_CODEC_ID_PCM_LXF:
                return 2 * (frame_bytes / (5 * ch));
            case AV_CODEC_ID_IAC:
            case AV_CODEC_ID_IMC:
                return 4 * frame_bytes / ch;
            }

            if (tag) {
                /* calc from frame_bytes, channels, and codec_tag */
                if (id == AV_CODEC_ID_SOL_DPCM) {
                    if (tag == 3)
                        return frame_bytes / ch;
                    else
                        return frame_bytes * 2 / ch;
                }
            }

            if (ba > 0) {
                /* calc from frame_bytes, channels, and block_align */
                int blocks = frame_bytes / ba;
                switch (id) {
                case AV_CODEC_ID_ADPCM_IMA_WAV:
                    if (bps < 2 || bps > 5)
                        return 0;
                    return blocks * (1 + (ba - 4 * ch) / (bps * ch) * 8);
                case AV_CODEC_ID_ADPCM_IMA_DK3:
                    return blocks * (((ba - 16) * 2 / 3 * 4) / ch);
                case AV_CODEC_ID_ADPCM_IMA_DK4:
                    return blocks * (1 + (ba - 4 * ch) * 2 / ch);
                case AV_CODEC_ID_ADPCM_IMA_RAD:
                    return blocks * ((ba - 4 * ch) * 2 / ch);
                case AV_CODEC_ID_ADPCM_MS:
                    return blocks * (2 + (ba - 7 * ch) * 2 / ch);
                case AV_CODEC_ID_ADPCM_MTAF:
                    return blocks * (ba - 16) * 2 / ch;
                }
            }

            if (bps > 0) {
                /* calc from frame_bytes, channels, and bits_per_coded_sample */
                switch (id) {
                case AV_CODEC_ID_PCM_DVD:
                    if(bps<4)
                        return 0;
                    return 2 * (frame_bytes / ((bps * 2 / 8) * ch));
                case AV_CODEC_ID_PCM_BLURAY:
                    if(bps<4)
                        return 0;
                    return frame_bytes / ((FFALIGN(ch, 2) * bps) / 8);
                case AV_CODEC_ID_S302M:
                    return 2 * (frame_bytes / ((bps + 4) / 4)) / ch;
                }
            }
        }
    }

    /* Fall back on using frame_size */
    if (frame_size > 1 && frame_bytes)
        return frame_size;

    if (bitrate > 0 && frame_bytes > 0 && sr > 0 && ba > 1) {
        if (id == AV_CODEC_ID_WMAV1 || id == AV_CODEC_ID_WMAV2)
            return  (frame_bytes * 8LL * sr) / bitrate;
    }

    return 0;
}
