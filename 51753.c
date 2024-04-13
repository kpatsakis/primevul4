dissect_dch_timing_adjustment(proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb, int offset)
{
    guint8      control_cfn;
    gint16      toa;
    proto_item *toa_ti;

    /* CFN control */
    control_cfn = tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_cfn_control, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* ToA */
    toa = tvb_get_ntohs(tvb, offset);
    toa_ti = proto_tree_add_item(tree, hf_fp_toa, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    expert_add_info_format(pinfo, toa_ti, &ei_fp_timing_adjustmentment_reported, "Timing adjustmentment reported (%f ms)", (float)(toa / 8));

    col_append_fstr(pinfo->cinfo, COL_INFO,
                    " CFN = %u, ToA = %d", control_cfn, toa);

    return offset;
}
