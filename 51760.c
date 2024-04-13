dissect_fach_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                          int offset, struct fp_info *p_fp_info, void *data)
{
    gboolean is_control_frame;
    guint16 header_crc = 0;
    proto_item * header_crc_pi = NULL;
    guint header_length = 0;

    /* Header CRC */
    header_crc = tvb_get_bits8(tvb, 0, 7);
    header_crc_pi = proto_tree_add_item(tree, hf_fp_header_crc, tvb, offset, 1, ENC_BIG_ENDIAN);

    /* Frame Type */
    is_control_frame = tvb_get_guint8(tvb, offset) & 0x01;
    proto_tree_add_item(tree, hf_fp_ft, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO, is_control_frame ? " [Control] " : " [Data] ");

    if (is_control_frame) {
        dissect_common_control(tvb, pinfo, tree, offset, p_fp_info);
        /* For control frame the header CRC is actually frame CRC covering all
         * bytes except the first */
        if (preferences_header_checksum) {
            verify_control_frame_crc(tvb, pinfo, header_crc_pi, header_crc);
        }
    }
    else {
        guint8 cfn;
        /* DATA */

        /* CFN */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%03u ", cfn);

        /* TFI */
        proto_tree_add_item(tree, hf_fp_fach_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* Transmit power level */
        proto_tree_add_float(tree, hf_fp_transmit_power_level, tvb, offset, 1,
                             (float)(int)(tvb_get_guint8(tvb, offset)) / 10);
        offset++;
        header_length = offset;

        /* TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, &mac_fdd_fach_handle, data);

        /* New IE flags (if it looks as though they are present) */
        if ((p_fp_info->release == 7) &&
            (tvb_reported_length_remaining(tvb, offset) > 2)) {

            guint8 flags = tvb_get_guint8(tvb, offset);
            guint8 aoa_present = flags & 0x01;
            offset++;

            if (aoa_present) {
                proto_tree_add_item(tree, hf_fp_angle_of_arrival, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
            }
        }
        if (preferences_header_checksum) {
            verify_header_crc(tvb, pinfo, header_crc_pi, header_crc, header_length);
        }
        /* Spare Extension and Payload CRC */
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}
