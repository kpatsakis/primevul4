static void decode_postinit(H264Context *h, int setup_finished)
{
    H264Picture *out = h->cur_pic_ptr;
    H264Picture *cur = h->cur_pic_ptr;
    int i, pics, out_of_order, out_idx;

    h->cur_pic_ptr->f.pict_type = h->pict_type;

    if (h->next_output_pic)
        return;

    if (cur->field_poc[0] == INT_MAX || cur->field_poc[1] == INT_MAX) {
        /* FIXME: if we have two PAFF fields in one packet, we can't start
         * the next thread here. If we have one field per packet, we can.
         * The check in decode_nal_units() is not good enough to find this
         * yet, so we assume the worst for now. */
        return;
    }

    cur->f.interlaced_frame = 0;
    cur->f.repeat_pict      = 0;

    /* Signal interlacing information externally. */
    /* Prioritize picture timing SEI information over used
     * decoding process if it exists. */

    if (h->sps.pic_struct_present_flag) {
        switch (h->sei_pic_struct) {
        case SEI_PIC_STRUCT_FRAME:
            break;
        case SEI_PIC_STRUCT_TOP_FIELD:
        case SEI_PIC_STRUCT_BOTTOM_FIELD:
            cur->f.interlaced_frame = 1;
            break;
        case SEI_PIC_STRUCT_TOP_BOTTOM:
        case SEI_PIC_STRUCT_BOTTOM_TOP:
            if (FIELD_OR_MBAFF_PICTURE(h))
                cur->f.interlaced_frame = 1;
            else
                cur->f.interlaced_frame = h->prev_interlaced_frame;
            break;
        case SEI_PIC_STRUCT_TOP_BOTTOM_TOP:
        case SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM:
            /* Signal the possibility of telecined film externally
             * (pic_struct 5,6). From these hints, let the applications
             * decide if they apply deinterlacing. */
            cur->f.repeat_pict = 1;
            break;
        case SEI_PIC_STRUCT_FRAME_DOUBLING:
            cur->f.repeat_pict = 2;
            break;
        case SEI_PIC_STRUCT_FRAME_TRIPLING:
            cur->f.repeat_pict = 4;
            break;
        }

        if ((h->sei_ct_type & 3) &&
            h->sei_pic_struct <= SEI_PIC_STRUCT_BOTTOM_TOP)
            cur->f.interlaced_frame = (h->sei_ct_type & (1 << 1)) != 0;
    } else {
        /* Derive interlacing flag from used decoding process. */
        cur->f.interlaced_frame = FIELD_OR_MBAFF_PICTURE(h);
    }
    h->prev_interlaced_frame = cur->f.interlaced_frame;

    if (cur->field_poc[0] != cur->field_poc[1]) {
        /* Derive top_field_first from field pocs. */
        cur->f.top_field_first = cur->field_poc[0] < cur->field_poc[1];
    } else {
        if (cur->f.interlaced_frame || h->sps.pic_struct_present_flag) {
            /* Use picture timing SEI information. Even if it is a
             * information of a past frame, better than nothing. */
            if (h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM ||
                h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM_TOP)
                cur->f.top_field_first = 1;
            else
                cur->f.top_field_first = 0;
        } else {
            /* Most likely progressive */
            cur->f.top_field_first = 0;
        }
    }

    if (h->sei_frame_packing_present &&
        h->frame_packing_arrangement_type >= 0 &&
        h->frame_packing_arrangement_type <= 6 &&
        h->content_interpretation_type > 0 &&
        h->content_interpretation_type < 3) {
        AVStereo3D *stereo = av_stereo3d_create_side_data(&cur->f);
        if (stereo) {
        switch (h->frame_packing_arrangement_type) {
        case 0:
            stereo->type = AV_STEREO3D_CHECKERBOARD;
            break;
        case 1:
            stereo->type = AV_STEREO3D_COLUMNS;
            break;
        case 2:
            stereo->type = AV_STEREO3D_LINES;
            break;
        case 3:
            if (h->quincunx_subsampling)
                stereo->type = AV_STEREO3D_SIDEBYSIDE_QUINCUNX;
            else
                stereo->type = AV_STEREO3D_SIDEBYSIDE;
            break;
        case 4:
            stereo->type = AV_STEREO3D_TOPBOTTOM;
            break;
        case 5:
            stereo->type = AV_STEREO3D_FRAMESEQUENCE;
            break;
        case 6:
            stereo->type = AV_STEREO3D_2D;
            break;
        }

        if (h->content_interpretation_type == 2)
            stereo->flags = AV_STEREO3D_FLAG_INVERT;
        }
    }

    if (h->sei_display_orientation_present &&
        (h->sei_anticlockwise_rotation || h->sei_hflip || h->sei_vflip)) {
        double angle = h->sei_anticlockwise_rotation * 360 / (double) (1 << 16);
        AVFrameSideData *rotation = av_frame_new_side_data(&cur->f,
                                                           AV_FRAME_DATA_DISPLAYMATRIX,
                                                           sizeof(int32_t) * 9);
        if (rotation) {
            av_display_rotation_set((int32_t *)rotation->data, angle);
            av_display_matrix_flip((int32_t *)rotation->data,
                                   h->sei_hflip, h->sei_vflip);
        }
    }

    cur->mmco_reset = h->mmco_reset;
    h->mmco_reset = 0;


    /* Sort B-frames into display order */

    if (h->sps.bitstream_restriction_flag &&
        h->avctx->has_b_frames < h->sps.num_reorder_frames) {
        h->avctx->has_b_frames = h->sps.num_reorder_frames;
        h->low_delay           = 0;
    }

    if (h->avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT &&
        !h->sps.bitstream_restriction_flag) {
        h->avctx->has_b_frames = MAX_DELAYED_PIC_COUNT - 1;
        h->low_delay           = 0;
    }

    for (i = 0; 1; i++) {
        if(i == MAX_DELAYED_PIC_COUNT || cur->poc < h->last_pocs[i]){
            if(i)
                h->last_pocs[i-1] = cur->poc;
            break;
        } else if(i) {
            h->last_pocs[i-1]= h->last_pocs[i];
        }
    }
    out_of_order = MAX_DELAYED_PIC_COUNT - i;
    if(   cur->f.pict_type == AV_PICTURE_TYPE_B
       || (h->last_pocs[MAX_DELAYED_PIC_COUNT-2] > INT_MIN && h->last_pocs[MAX_DELAYED_PIC_COUNT-1] - h->last_pocs[MAX_DELAYED_PIC_COUNT-2] > 2))
        out_of_order = FFMAX(out_of_order, 1);
    if (out_of_order == MAX_DELAYED_PIC_COUNT) {
        av_log(h->avctx, AV_LOG_VERBOSE, "Invalid POC %d<%d\n", cur->poc, h->last_pocs[0]);
        for (i = 1; i < MAX_DELAYED_PIC_COUNT; i++)
            h->last_pocs[i] = INT_MIN;
        h->last_pocs[0] = cur->poc;
        cur->mmco_reset = 1;
    } else if(h->avctx->has_b_frames < out_of_order && !h->sps.bitstream_restriction_flag){
        av_log(h->avctx, AV_LOG_VERBOSE, "Increasing reorder buffer to %d\n", out_of_order);
        h->avctx->has_b_frames = out_of_order;
        h->low_delay = 0;
    }

    pics = 0;
    while (h->delayed_pic[pics])
        pics++;

    av_assert0(pics <= MAX_DELAYED_PIC_COUNT);

    h->delayed_pic[pics++] = cur;
    if (cur->reference == 0)
        cur->reference = DELAYED_PIC_REF;

    out     = h->delayed_pic[0];
    out_idx = 0;
    for (i = 1; h->delayed_pic[i] &&
                !h->delayed_pic[i]->f.key_frame &&
                !h->delayed_pic[i]->mmco_reset;
         i++)
        if (h->delayed_pic[i]->poc < out->poc) {
            out     = h->delayed_pic[i];
            out_idx = i;
        }
    if (h->avctx->has_b_frames == 0 &&
        (h->delayed_pic[0]->f.key_frame || h->delayed_pic[0]->mmco_reset))
        h->next_outputed_poc = INT_MIN;
    out_of_order = out->poc < h->next_outputed_poc;

    if (out_of_order || pics > h->avctx->has_b_frames) {
        out->reference &= ~DELAYED_PIC_REF;
        for (i = out_idx; h->delayed_pic[i]; i++)
            h->delayed_pic[i] = h->delayed_pic[i + 1];
    }
    if (!out_of_order && pics > h->avctx->has_b_frames) {
        h->next_output_pic = out;
        if (out_idx == 0 && h->delayed_pic[0] && (h->delayed_pic[0]->f.key_frame || h->delayed_pic[0]->mmco_reset)) {
            h->next_outputed_poc = INT_MIN;
        } else
            h->next_outputed_poc = out->poc;
    } else {
        av_log(h->avctx, AV_LOG_DEBUG, "no picture %s\n", out_of_order ? "ooo" : "");
    }

    if (h->next_output_pic) {
        if (h->next_output_pic->recovered) {
            h->frame_recovered |= FRAME_RECOVERED_SEI;
        }
        h->next_output_pic->recovered |= !!(h->frame_recovered & FRAME_RECOVERED_SEI);
    }

    if (setup_finished && !h->avctx->hwaccel)
        ff_thread_finish_setup(h->avctx);
}
