dissect_dsch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
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
        guint8 cfn;
        guint header_length = 0;

        /* DATA */

        /* CFN */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%03u ", cfn);

        /* TFI */
        proto_tree_add_item(tree, hf_fp_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;


        /* Other fields depend upon release & FDD/TDD settings */
        if (((p_fp_info->release == 99) || (p_fp_info->release == 4)) &&
             (p_fp_info->channel == CHANNEL_DSCH_FDD)) {

            /* Power offset */
            proto_tree_add_float(tree, hf_fp_power_offset, tvb, offset, 1,
                                 (float)(-32.0) +
                                  ((float)(int)(tvb_get_guint8(tvb, offset)) * (float)(0.25)));
            offset++;

            /* Code number */
            proto_tree_add_item(tree, hf_fp_code_number, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;

            /* Spreading Factor (3 bits) */
            proto_tree_add_item(tree, hf_fp_spreading_factor, tvb, offset, 1, ENC_BIG_ENDIAN);

            /* MC info (4 bits)*/
            proto_tree_add_item(tree, hf_fp_mc_info, tvb, offset, 1, ENC_BIG_ENDIAN);

            /* Last bit of this byte is spare */
            offset++;
        }
        else {
            /* Normal case */

            /* PDSCH Set Id */
            proto_tree_add_item(tree, hf_fp_pdsch_set_id, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;

            /* Transmit power level */
            proto_tree_add_float(tree, hf_fp_transmit_power_level, tvb, offset, 1,
                                 (float)(int)(tvb_get_guint8(tvb, offset)) / 10);
            offset++;
        }
        header_length = offset;
        /* TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, NULL, NULL);

        /* Spare Extension and Payload CRC */
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}
