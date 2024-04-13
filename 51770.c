dissect_pch_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                         int offset, struct fp_info *p_fp_info, void *data)
{
    gboolean is_control_frame;
    guint16  pch_cfn;
    gboolean paging_indication;
    guint16 header_crc = 0;
    proto_item * header_crc_pi = NULL;

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
        guint header_length = 0;
        /* DATA */

        /* 12-bit CFN value */
        proto_tree_add_item(tree, hf_fp_pch_cfn, tvb, offset, 2, ENC_BIG_ENDIAN);
        pch_cfn = (tvb_get_ntohs(tvb, offset) & 0xfff0) >> 4;
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%04u ", pch_cfn);

        /* Paging indication */
        proto_tree_add_item(tree, hf_fp_pch_pi, tvb, offset, 1, ENC_BIG_ENDIAN);
        paging_indication = tvb_get_guint8(tvb, offset) & 0x01;
        offset++;

        /* 5-bit TFI */
        proto_tree_add_item(tree, hf_fp_pch_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;
        header_length = offset;
        /* Optional paging indications */
        if (paging_indication) {
            proto_item *ti;
            ti = proto_tree_add_item(tree, hf_fp_paging_indication_bitmap, tvb,
                                     offset,
                                     (p_fp_info->paging_indications+7) / 8,
                                     ENC_NA);
            proto_item_append_text(ti, " (%u bits)", p_fp_info->paging_indications);
            offset += ((p_fp_info->paging_indications+7) / 8);
        }

        /* TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, &mac_fdd_pch_handle, data);

        if (preferences_header_checksum) {
            verify_header_crc(tvb, pinfo, header_crc_pi, header_crc, header_length);
        }
        /* Spare Extension and Payload CRC */
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}
