dissect_usch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
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
        guint16 rx_timing_deviation;
        proto_item *rx_timing_deviation_ti;
        guint header_length = 0;

        /* DATA */

        /* CFN */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%03u ", cfn);

        /* TFI */
        proto_tree_add_item(tree, hf_fp_usch_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* Rx Timing Deviation */
        rx_timing_deviation = tvb_get_guint8(tvb, offset);
        rx_timing_deviation_ti = proto_tree_add_item(tree, hf_fp_rx_timing_deviation,
                                                     tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;
        header_length = offset;
        /* TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, NULL, NULL);

        /* QE */
        proto_tree_add_item(tree, hf_fp_quality_estimate, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* CRCIs */
        offset = dissect_crci_bits(tvb, pinfo, tree, p_fp_info, offset);

        /* New IEs */
        if ((p_fp_info->release == 7) &&
            (tvb_reported_length_remaining(tvb, offset) > 2)) {

            guint8 flags = tvb_get_guint8(tvb, offset);
            guint8 bits_extended = flags & 0x01;
            offset++;

            if (bits_extended) {
                guint8 extra_bits = tvb_get_guint8(tvb, offset) & 0x03;
                proto_item_append_text(rx_timing_deviation_ti,
                                       " (extended to %u)",
                                       (rx_timing_deviation << 2) | extra_bits);
            }
            offset++;
        }

        /* Spare Extension and Payload CRC */
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}
