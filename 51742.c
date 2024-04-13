dissect_common_timing_adjustment(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb,
                                 int offset, struct fp_info *p_fp_info)
{
    if (p_fp_info->channel != CHANNEL_PCH) {
        guint8 cfn;
        gint16 toa;

        /* CFN control */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn_control, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* ToA */
        toa = tvb_get_ntohs(tvb, offset);
        proto_tree_add_item(tree, hf_fp_toa, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;

        col_append_fstr(pinfo->cinfo, COL_INFO, "   CFN=%u, ToA=%d", cfn, toa);
    }
    else {
        guint16 cfn;
        gint32 toa;

        /* PCH CFN is 12 bits */
        cfn = (tvb_get_ntohs(tvb, offset) >> 4);
        proto_tree_add_item(tree, hf_fp_pch_cfn, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;

        /* 4 bits of padding follow... */

        /* 20 bits of ToA (followed by 4 padding bits) */
        toa = ((int)(tvb_get_ntoh24(tvb, offset) << 8)) / 4096;
        proto_tree_add_int(tree, hf_fp_pch_toa, tvb, offset, 3, toa);
        offset += 3;

        col_append_fstr(pinfo->cinfo, COL_INFO, "   CFN=%u, ToA=%d", cfn, toa);
    }
    return offset;
}
