static int decode_nal_units(H264Context *h, const uint8_t *buf, int buf_size,
                            int parse_extradata)
{
    AVCodecContext *const avctx = h->avctx;
    H264Context *hx; ///< thread context
    int buf_index;
    unsigned context_count;
    int next_avc;
    int nals_needed = 0; ///< number of NALs that need decoding before the next frame thread starts
    int nal_index;
    int idr_cleared=0;
    int ret = 0;

    h->nal_unit_type= 0;

    if(!h->slice_context_count)
         h->slice_context_count= 1;
    h->max_contexts = h->slice_context_count;
    if (!(avctx->flags2 & CODEC_FLAG2_CHUNKS)) {
        h->current_slice = 0;
        if (!h->first_field)
            h->cur_pic_ptr = NULL;
        ff_h264_reset_sei(h);
    }

    if (h->nal_length_size == 4) {
        if (buf_size > 8 && AV_RB32(buf) == 1 && AV_RB32(buf+5) > (unsigned)buf_size) {
            h->is_avc = 0;
        }else if(buf_size > 3 && AV_RB32(buf) > 1 && AV_RB32(buf) <= (unsigned)buf_size)
            h->is_avc = 1;
    }

    if (avctx->active_thread_type & FF_THREAD_FRAME)
        nals_needed = get_last_needed_nal(h, buf, buf_size);

    {
        buf_index     = 0;
        context_count = 0;
        next_avc      = h->is_avc ? 0 : buf_size;
        nal_index     = 0;
        for (;;) {
            int consumed;
            int dst_length;
            int bit_length;
            const uint8_t *ptr;
            int nalsize = 0;
            int err;

            if (buf_index >= next_avc) {
                nalsize = get_avc_nalsize(h, buf, buf_size, &buf_index);
                if (nalsize < 0)
                    break;
                next_avc = buf_index + nalsize;
            } else {
                buf_index = find_start_code(buf, buf_size, buf_index, next_avc);
                if (buf_index >= buf_size)
                    break;
                if (buf_index >= next_avc)
                    continue;
            }

            hx = h->thread_context[context_count];

            ptr = ff_h264_decode_nal(hx, buf + buf_index, &dst_length,
                                     &consumed, next_avc - buf_index);
            if (!ptr || dst_length < 0) {
                ret = -1;
                goto end;
            }

            bit_length = get_bit_length(h, buf, ptr, dst_length,
                                        buf_index + consumed, next_avc);

            if (h->avctx->debug & FF_DEBUG_STARTCODE)
                av_log(h->avctx, AV_LOG_DEBUG,
                       "NAL %d/%d at %d/%d length %d\n",
                       hx->nal_unit_type, hx->nal_ref_idc, buf_index, buf_size, dst_length);

            if (h->is_avc && (nalsize != consumed) && nalsize)
                av_log(h->avctx, AV_LOG_DEBUG,
                       "AVC: Consumed only %d bytes instead of %d\n",
                       consumed, nalsize);

            buf_index += consumed;
            nal_index++;

            if (avctx->skip_frame >= AVDISCARD_NONREF &&
                h->nal_ref_idc == 0 &&
                h->nal_unit_type != NAL_SEI)
                continue;

again:
            if (   !(avctx->active_thread_type & FF_THREAD_FRAME)
                || nals_needed >= nal_index)
                h->au_pps_id = -1;
            /* Ignore per frame NAL unit type during extradata
             * parsing. Decoding slices is not possible in codec init
             * with frame-mt */
            if (parse_extradata) {
                switch (hx->nal_unit_type) {
                case NAL_IDR_SLICE:
                case NAL_SLICE:
                case NAL_DPA:
                case NAL_DPB:
                case NAL_DPC:
                    av_log(h->avctx, AV_LOG_WARNING,
                           "Ignoring NAL %d in global header/extradata\n",
                           hx->nal_unit_type);
                case NAL_AUXILIARY_SLICE:
                    hx->nal_unit_type = NAL_FF_IGNORE;
                }
            }

            err = 0;

            switch (hx->nal_unit_type) {
            case NAL_IDR_SLICE:
                if ((ptr[0] & 0xFC) == 0x98) {
                    av_log(h->avctx, AV_LOG_ERROR, "Invalid inter IDR frame\n");
                    h->next_outputed_poc = INT_MIN;
                    ret = -1;
                    goto end;
                }
                if (h->nal_unit_type != NAL_IDR_SLICE) {
                    av_log(h->avctx, AV_LOG_ERROR,
                           "Invalid mix of idr and non-idr slices\n");
                    ret = -1;
                    goto end;
                }
                if(!idr_cleared)
                    idr(h); // FIXME ensure we don't lose some frames if there is reordering
                idr_cleared = 1;
                h->has_recovery_point = 1;
            case NAL_SLICE:
                init_get_bits(&hx->gb, ptr, bit_length);
                hx->intra_gb_ptr      =
                hx->inter_gb_ptr      = &hx->gb;
                hx->data_partitioning = 0;

                if ((err = ff_h264_decode_slice_header(hx, h)))
                    break;

                if (h->sei_recovery_frame_cnt >= 0) {
                    if (h->frame_num != h->sei_recovery_frame_cnt || hx->slice_type_nos != AV_PICTURE_TYPE_I)
                        h->valid_recovery_point = 1;

                    if (   h->recovery_frame < 0
                        || ((h->recovery_frame - h->frame_num) & ((1 << h->sps.log2_max_frame_num)-1)) > h->sei_recovery_frame_cnt) {
                        h->recovery_frame = (h->frame_num + h->sei_recovery_frame_cnt) &
                                            ((1 << h->sps.log2_max_frame_num) - 1);

                        if (!h->valid_recovery_point)
                            h->recovery_frame = h->frame_num;
                    }
                }

                h->cur_pic_ptr->f.key_frame |=
                    (hx->nal_unit_type == NAL_IDR_SLICE);

                if (hx->nal_unit_type == NAL_IDR_SLICE ||
                    h->recovery_frame == h->frame_num) {
                    h->recovery_frame         = -1;
                    h->cur_pic_ptr->recovered = 1;
                }
                if (hx->nal_unit_type == NAL_IDR_SLICE)
                    h->frame_recovered |= FRAME_RECOVERED_IDR;
                h->frame_recovered |= 3*!!(avctx->flags2 & CODEC_FLAG2_SHOW_ALL);
                h->frame_recovered |= 3*!!(avctx->flags & CODEC_FLAG_OUTPUT_CORRUPT);
#if 1
                h->cur_pic_ptr->recovered |= h->frame_recovered;
#else
                h->cur_pic_ptr->recovered |= !!(h->frame_recovered & FRAME_RECOVERED_IDR);
#endif

                if (h->current_slice == 1) {
                    if (!(avctx->flags2 & CODEC_FLAG2_CHUNKS))
                        decode_postinit(h, nal_index >= nals_needed);

                    if (h->avctx->hwaccel &&
                        (ret = h->avctx->hwaccel->start_frame(h->avctx, NULL, 0)) < 0)
                        return ret;
                    if (CONFIG_H264_VDPAU_DECODER &&
                        h->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
                        ff_vdpau_h264_picture_start(h);
                }

                if (hx->redundant_pic_count == 0) {
                    if (avctx->hwaccel) {
                        ret = avctx->hwaccel->decode_slice(avctx,
                                                           &buf[buf_index - consumed],
                                                           consumed);
                        if (ret < 0)
                            return ret;
                    } else if (CONFIG_H264_VDPAU_DECODER &&
                               h->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU) {
                        ff_vdpau_add_data_chunk(h->cur_pic_ptr->f.data[0],
                                                start_code,
                                                sizeof(start_code));
                        ff_vdpau_add_data_chunk(h->cur_pic_ptr->f.data[0],
                                                &buf[buf_index - consumed],
                                                consumed);
                    } else
                        context_count++;
                }
                break;
            case NAL_DPA:
                if (h->avctx->flags & CODEC_FLAG2_CHUNKS) {
                    av_log(h->avctx, AV_LOG_ERROR,
                           "Decoding in chunks is not supported for "
                           "partitioned slices.\n");
                    return AVERROR(ENOSYS);
                }

                init_get_bits(&hx->gb, ptr, bit_length);
                hx->intra_gb_ptr =
                hx->inter_gb_ptr = NULL;

                if ((err = ff_h264_decode_slice_header(hx, h))) {
                    /* make sure data_partitioning is cleared if it was set
                     * before, so we don't try decoding a slice without a valid
                     * slice header later */
                    h->data_partitioning = 0;
                    break;
                }

                hx->data_partitioning = 1;
                break;
            case NAL_DPB:
                init_get_bits(&hx->intra_gb, ptr, bit_length);
                hx->intra_gb_ptr = &hx->intra_gb;
                break;
            case NAL_DPC:
                init_get_bits(&hx->inter_gb, ptr, bit_length);
                hx->inter_gb_ptr = &hx->inter_gb;

                av_log(h->avctx, AV_LOG_ERROR, "Partitioned H.264 support is incomplete\n");
                break;

                if (hx->redundant_pic_count == 0 &&
                    hx->intra_gb_ptr &&
                    hx->data_partitioning &&
                    h->cur_pic_ptr && h->context_initialized &&
                    (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc) &&
                    (avctx->skip_frame < AVDISCARD_BIDIR  ||
                     hx->slice_type_nos != AV_PICTURE_TYPE_B) &&
                    (avctx->skip_frame < AVDISCARD_NONINTRA ||
                     hx->slice_type_nos == AV_PICTURE_TYPE_I) &&
                    avctx->skip_frame < AVDISCARD_ALL)
                    context_count++;
                break;
            case NAL_SEI:
                init_get_bits(&h->gb, ptr, bit_length);
                ret = ff_h264_decode_sei(h);
                if (ret < 0 && (h->avctx->err_recognition & AV_EF_EXPLODE))
                    goto end;
                break;
            case NAL_SPS:
                init_get_bits(&h->gb, ptr, bit_length);
                if (ff_h264_decode_seq_parameter_set(h) < 0 && (h->is_avc ? nalsize : 1)) {
                    av_log(h->avctx, AV_LOG_DEBUG,
                           "SPS decoding failure, trying again with the complete NAL\n");
                    if (h->is_avc)
                        av_assert0(next_avc - buf_index + consumed == nalsize);
                    if ((next_avc - buf_index + consumed - 1) >= INT_MAX/8)
                        break;
                    init_get_bits(&h->gb, &buf[buf_index + 1 - consumed],
                                  8*(next_avc - buf_index + consumed - 1));
                    ff_h264_decode_seq_parameter_set(h);
                }

                break;
            case NAL_PPS:
                init_get_bits(&h->gb, ptr, bit_length);
                ret = ff_h264_decode_picture_parameter_set(h, bit_length);
                if (ret < 0 && (h->avctx->err_recognition & AV_EF_EXPLODE))
                    goto end;
                break;
            case NAL_AUD:
            case NAL_END_SEQUENCE:
            case NAL_END_STREAM:
            case NAL_FILLER_DATA:
            case NAL_SPS_EXT:
            case NAL_AUXILIARY_SLICE:
                break;
            case NAL_FF_IGNORE:
                break;
            default:
                av_log(avctx, AV_LOG_DEBUG, "Unknown NAL code: %d (%d bits)\n",
                       hx->nal_unit_type, bit_length);
            }

            if (context_count == h->max_contexts) {
                ret = ff_h264_execute_decode_slices(h, context_count);
                if (ret < 0 && (h->avctx->err_recognition & AV_EF_EXPLODE))
                    goto end;
                context_count = 0;
            }

            if (err < 0 || err == SLICE_SKIPED) {
                if (err < 0)
                    av_log(h->avctx, AV_LOG_ERROR, "decode_slice_header error\n");
                h->ref_count[0] = h->ref_count[1] = h->list_count = 0;
            } else if (err == SLICE_SINGLETHREAD) {
                /* Slice could not be decoded in parallel mode, copy down
                 * NAL unit stuff to context 0 and restart. Note that
                 * rbsp_buffer is not transferred, but since we no longer
                 * run in parallel mode this should not be an issue. */
                h->nal_unit_type = hx->nal_unit_type;
                h->nal_ref_idc   = hx->nal_ref_idc;
                hx               = h;
                goto again;
            }
        }
    }
    if (context_count) {
        ret = ff_h264_execute_decode_slices(h, context_count);
        if (ret < 0 && (h->avctx->err_recognition & AV_EF_EXPLODE))
            goto end;
    }

    ret = 0;
end:
    /* clean up */
    if (h->cur_pic_ptr && !h->droppable) {
        ff_thread_report_progress(&h->cur_pic_ptr->tf, INT_MAX,
                                  h->picture_structure == PICT_BOTTOM_FIELD);
    }

    return (ret < 0) ? ret : buf_index;
}
