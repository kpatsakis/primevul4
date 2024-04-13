dissect_cpch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
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
        guint cfn;
        guint header_length = 0;
        /* DATA */

        /* CFN */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%03u ", cfn);

        /* TFI */
        proto_tree_add_item(tree, hf_fp_cpch_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* Propagation delay */
        proto_tree_add_uint(tree, hf_fp_propagation_delay, tvb, offset, 1,
                            tvb_get_guint8(tvb, offset) * 3);
        offset++;
        header_length = offset; /* XXX this might be wrong */
        /* TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, NULL, NULL);

        /* CRCIs */
        offset = dissect_crci_bits(tvb, pinfo, tree, p_fp_info, offset);

        /* Spare Extension and Payload CRC */
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}
