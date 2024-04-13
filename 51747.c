dissect_dch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                         int offset, struct fp_info *p_fp_info, void *data)
{
    gboolean is_control_frame;
    guint8   cfn;
    guint header_length = 0;
    guint16 header_crc = 0;
    proto_item * header_crc_pi = NULL;

    /* Header CRC */
    header_crc = tvb_get_bits8(tvb, 0, 7);
    header_crc_pi = proto_tree_add_item(tree, hf_fp_header_crc, tvb, offset, 1, ENC_BIG_ENDIAN);

    /* Frame Type */
    is_control_frame = tvb_get_guint8(tvb, offset) & 0x01;
    proto_tree_add_item(tree, hf_fp_ft, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO,
                   is_control_frame ? " [Control] " :
                                      ((p_fp_info->is_uplink) ? " [ULData] " :
                                                                " [DLData] " ));

    if (is_control_frame) {
        /* DCH control frame */
        dissect_dch_control_frame(tree, pinfo, tvb, offset, p_fp_info);
        /* For control frame the header CRC is actually frame CRC covering all
         * bytes except the first */
        if (preferences_header_checksum) {
            verify_control_frame_crc(tvb, pinfo, header_crc_pi, header_crc);
        }
    } else {
        /************************/
        /* DCH data here        */
        int chan;
        /* CFN */
        proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
        cfn = tvb_get_guint8(tvb, offset);
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%03u ", cfn);

        /* One TFI for each channel */
        for (chan=0; chan < p_fp_info->num_chans; chan++) {
            proto_tree_add_item(tree, hf_fp_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
        }
        header_length = offset;
        /* Dissect TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, &mac_fdd_dch_handle, data);

        /* QE (uplink only) */
        if (p_fp_info->is_uplink) {
            proto_tree_add_item(tree, hf_fp_quality_estimate, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
        }

        /* CRCI bits (uplink only) */
        if (p_fp_info->is_uplink) {
            offset = dissect_crci_bits(tvb, pinfo, tree, p_fp_info, offset);
        }
        if (preferences_header_checksum) {
            verify_header_crc(tvb, pinfo, header_crc_pi, header_crc, header_length);
        }
        /* Spare extension and payload CRC (optional) */
        dissect_spare_extension_and_crc(tvb, pinfo, tree,
                                        p_fp_info->dch_crc_present, offset, header_length);
    }
}
