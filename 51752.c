dissect_dch_rx_timing_deviation(packet_info *pinfo, proto_tree *tree,
                                tvbuff_t *tvb, int offset,
                                struct fp_info *p_fp_info)
{
    guint16     timing_deviation;
    gint        timing_deviation_chips;
    proto_item *timing_deviation_ti;

    /* CFN control */
    proto_tree_add_item(tree, hf_fp_cfn_control, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* Rx Timing Deviation */
    timing_deviation = tvb_get_guint8(tvb, offset);
    timing_deviation_ti = proto_tree_add_item(tree, hf_fp_dch_rx_timing_deviation, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* May be extended in R7, but in this case there are at least 2 bytes remaining */
    if ((p_fp_info->release == 7) &&
        (tvb_reported_length_remaining(tvb, offset) >= 2)) {

        /* New IE flags */
        guint64 extended_bits_present;
        guint64 e_rucch_present;

        /* Read flags */
        proto_tree_add_bits_ret_val(tree, hf_fp_e_rucch_present, tvb,
                                    offset*8 + 6, 1, &e_rucch_present, ENC_BIG_ENDIAN);
        proto_tree_add_bits_ret_val(tree, hf_fp_extended_bits_present, tvb,
                                    offset*8 + 7, 1, &extended_bits_present, ENC_BIG_ENDIAN);
        offset++;

        /* Optional E-RUCCH */
        if (e_rucch_present) {

            /* Value of bit_offset depends upon division type */
            int bit_offset;

            switch (p_fp_info->division) {
                case Division_TDD_384:
                    bit_offset = 6;
                    break;
                case Division_TDD_768:
                    bit_offset = 5;
                    break;
                default:
                    {
                        proto_tree_add_expert(tree, pinfo, &ei_fp_expecting_tdd, tvb, 0, 0);
                        bit_offset = 6;
                    }
            }

            proto_tree_add_item(tree, hf_fp_dch_e_rucch_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
            proto_tree_add_bits_item(tree, hf_fp_dch_e_rucch_flag, tvb,
                                     offset*8 + bit_offset, 1, ENC_BIG_ENDIAN);
        }

        /* Timing deviation may be extended by another:
           - 1 bits (3.84 TDD)    OR
           - 2 bits (7.68 TDD)
        */
        if (extended_bits_present) {
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
            timing_deviation = (extra_bits << 8) | (timing_deviation);
            proto_item_append_text(timing_deviation_ti,
                                   " (extended to 0x%x)",
                                   timing_deviation);
            proto_tree_add_bits_item(tree, hf_fp_extended_bits, tvb,
                                     offset*8 + (8-bits_to_extend), bits_to_extend, ENC_BIG_ENDIAN);
            offset++;
        }
    }

    timing_deviation_chips = (timing_deviation*4) - 1024;
    proto_item_append_text(timing_deviation_ti, " (%d chips)",
                           timing_deviation_chips);

    col_append_fstr(pinfo->cinfo, COL_INFO, " deviation = %u (%d chips)",
                    timing_deviation, timing_deviation_chips);

    return offset;
}
