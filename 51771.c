dissect_rach_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
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
        guint8      cfn;
        guint32     propagation_delay                    = 0;
        proto_item *propagation_delay_ti                 = NULL;
        guint32     received_sync_ul_timing_deviation    = 0;
        proto_item *received_sync_ul_timing_deviation_ti = NULL;
        proto_item *rx_timing_deviation_ti               = NULL;
        guint16     rx_timing_deviation                  = 0;

        /* DATA */

        /* CFN */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        col_append_fstr(pinfo->cinfo, COL_INFO, "CFN=%03u ", cfn);

        /* TFI */
        proto_tree_add_item(tree, hf_fp_tfi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        if (p_fp_info->channel == CHANNEL_RACH_FDD) {
            /* Propagation delay */
            propagation_delay = tvb_get_guint8(tvb, offset);
            propagation_delay_ti = proto_tree_add_uint(tree, hf_fp_propagation_delay, tvb, offset, 1,
                                                       propagation_delay*3);
            offset++;
        }

        /* Should be TDD 3.84 or 7.68 */
        if (p_fp_info->channel == CHANNEL_RACH_TDD) {
            /* Rx Timing Deviation */
            rx_timing_deviation = tvb_get_guint8(tvb, offset);
            rx_timing_deviation_ti = proto_tree_add_item(tree, hf_fp_rx_timing_deviation, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
        }

        if (p_fp_info->channel == CHANNEL_RACH_TDD_128) {
            /* Received SYNC UL Timing Deviation */
            received_sync_ul_timing_deviation = tvb_get_guint8(tvb, offset);
            received_sync_ul_timing_deviation_ti =
                 proto_tree_add_item(tree, hf_fp_received_sync_ul_timing_deviation, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
        }

        header_length = offset;

        /* TB data */
        offset = dissect_tb_data(tvb, pinfo, tree, offset, p_fp_info, &mac_fdd_rach_handle, data);

        /* CRCIs */
        offset = dissect_crci_bits(tvb, pinfo, tree, p_fp_info, offset);

        /* Info introduced in R6 */
        /* only check if it looks as if they are present */
        if (((p_fp_info->release == 6) || (p_fp_info->release == 7)) &&
            (tvb_reported_length_remaining(tvb, offset) > 2))
        {
            int n;
            guint8 flags;
            /* guint8 flag_bytes = 0; */

            gboolean cell_portion_id_present                 = FALSE;
            gboolean ext_propagation_delay_present           = FALSE;
            gboolean angle_of_arrival_present                = FALSE;
            gboolean ext_rx_sync_ul_timing_deviation_present = FALSE;
            gboolean ext_rx_timing_deviation_present         = FALSE;

            /* New IE flags (assume mandatory for now) */
            do {
                proto_item *new_ie_flags_ti;
                proto_tree *new_ie_flags_tree;
                guint ies_found = 0;

                /* Add new IE flags subtree */
                new_ie_flags_ti = proto_tree_add_string_format(tree, hf_fp_rach_new_ie_flags, tvb, offset, 1,
                                                              "", "New IE flags");
                new_ie_flags_tree = proto_item_add_subtree(new_ie_flags_ti, ett_fp_rach_new_ie_flags);

                /* Read next byte */
                flags = tvb_get_guint8(tvb, offset);
                /* flag_bytes++ */

                /* Dissect individual bits */
                for (n=0; n < 8; n++) {
                    switch (n) {
                        case 6:
                            switch (p_fp_info->division) {
                                case Division_FDD:
                                    /* Ext propagation delay */
                                    ext_propagation_delay_present = TRUE;
                                    proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_ext_propagation_delay_present,
                                                        tvb, offset, 1, ENC_BIG_ENDIAN);
                                    break;
                                case Division_TDD_128:
                                    /* Ext Rx Sync UL Timing */
                                    ext_rx_sync_ul_timing_deviation_present = TRUE;
                                    proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_ext_rx_sync_ul_timing_deviation_present,
                                                        tvb, offset, 1, ENC_BIG_ENDIAN);

                                    break;
                                default:
                                    /* Not defined */
                                    proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_new_ie_flag_unused[6],
                                                        tvb, offset, 1, ENC_BIG_ENDIAN);
                                    break;
                            }
                            break;
                        case 7:
                            switch (p_fp_info->division) {
                                case Division_FDD:
                                    /* Cell Portion ID */
                                    cell_portion_id_present = TRUE;
                                    proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_cell_portion_id_present,
                                                        tvb, offset, 1, ENC_BIG_ENDIAN);
                                    break;
                                case Division_TDD_128:
                                    /* AOA */
                                    angle_of_arrival_present = TRUE;
                                    proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_angle_of_arrival_present,
                                                        tvb, offset, 1, ENC_BIG_ENDIAN);
                                    break;
                                case Division_TDD_384:
                                case Division_TDD_768:
                                    /* Extended Rx Timing Deviation */
                                    ext_rx_timing_deviation_present = TRUE;
                                    proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_ext_rx_timing_deviation_present,
                                                        tvb, offset, 1, ENC_BIG_ENDIAN);
                                    break;
                            }
                            break;

                        default:
                            /* No defined meanings */
                            /* Visual Studio Code Analyzer wrongly thinks n can be 7 here. It can't */
                            proto_tree_add_item(new_ie_flags_tree, hf_fp_rach_new_ie_flag_unused[n],
                                                tvb, offset, 1, ENC_BIG_ENDIAN);
                            break;
                    }
                    if ((flags >> (7-n)) & 0x01) {
                        ies_found++;
                    }
                }
                offset++;

                proto_item_append_text(new_ie_flags_ti, " (%u IEs found)", ies_found);

                /* Last bit set will indicate another flags byte follows... */
            } while (0); /*((flags & 0x01) && (flag_bytes < 31));*/

            /* Cell Portion ID */
            if (cell_portion_id_present) {
                    proto_tree_add_item(tree, hf_fp_cell_portion_id, tvb, offset, 1, ENC_BIG_ENDIAN);
                    offset++;
            }

            /* Ext Rx Timing Deviation */
            if (ext_rx_timing_deviation_present) {
                guint8 extra_bits;
                guint bits_to_extend;
                switch (p_fp_info->division) {
                    case Division_TDD_384:
                        bits_to_extend = 1;
                        break;
                    case Division_TDD_768:
                        bits_to_extend = 2;
                        break;

                    default:
                        /* TODO: report unexpected division type */
                        bits_to_extend = 1;
                        break;
                }
                extra_bits = tvb_get_guint8(tvb, offset) &
                                 ((bits_to_extend == 1) ? 0x01 : 0x03);
                rx_timing_deviation = (extra_bits << 8) | (rx_timing_deviation);
                proto_item_append_text(rx_timing_deviation_ti,
                                       " (extended to 0x%x)",
                                       rx_timing_deviation);
                proto_tree_add_bits_item(tree, hf_fp_extended_bits, tvb,
                                         offset*8 + (8-bits_to_extend), bits_to_extend, ENC_BIG_ENDIAN);
                offset++;
            }

            /* Ext propagation delay. */
            if (ext_propagation_delay_present) {
                guint16 extra_bits = tvb_get_ntohs(tvb, offset) & 0x03ff;
                proto_tree_add_item(tree, hf_fp_ext_propagation_delay, tvb, offset, 2, ENC_BIG_ENDIAN);

                /* Adding 10 bits to original 8 */
                proto_item_append_text(propagation_delay_ti, " (extended to %u)",
                                       ((extra_bits << 8) | propagation_delay) * 3);
                offset += 2;
            }

            /* Angle of Arrival (AOA) */
            if (angle_of_arrival_present) {
                proto_tree_add_item(tree, hf_fp_angle_of_arrival, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
            }

            /* Ext. Rx Sync UL Timing Deviation */
            if (ext_rx_sync_ul_timing_deviation_present) {
                guint16 extra_bits;

                /* Ext received Sync UL Timing Deviation */
                extra_bits = tvb_get_ntohs(tvb, offset) & 0x1fff;
                proto_tree_add_item(tree, hf_fp_ext_received_sync_ul_timing_deviation, tvb, offset, 2, ENC_BIG_ENDIAN);

                /* Adding 13 bits to original 8 */
                proto_item_append_text(received_sync_ul_timing_deviation_ti, " (extended to %u)",
                                       (extra_bits << 8) | received_sync_ul_timing_deviation);
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
