dissect_iur_dsch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                              int offset, struct fp_info *p_fp_info)
{
    gboolean is_control_frame;

    /* Header CRC */
    proto_tree_add_item(tree, hf_fp_header_crc, tvb, offset, 1, ENC_BIG_ENDIAN);

    /* Frame Type */
    is_control_frame = tvb_get_guint8(tvb, offset) & 0x01;
    proto_tree_add_item(tree, hf_fp_ft, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO, is_control_frame ? " [Control] " : " [Data] ");

    if (is_control_frame) {
        dissect_common_control(tvb, pinfo, tree, offset, p_fp_info);
    }
    else {
        /* TODO: DATA */
    }
}
