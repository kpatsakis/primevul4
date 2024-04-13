dissect_hsdpa_capacity_allocation_type_2(packet_info *pinfo, proto_tree *tree,
                                         tvbuff_t *tvb, int offset)
{
    proto_item *ti;
    proto_item *rate_ti;
    guint16     max_pdu_length;
    guint8      repetition_period;
    guint8      interval;
    guint16     credits;

    /* Congestion status */
    proto_tree_add_bits_item(tree, hf_fp_congestion_status, tvb,
                            offset*8 + 2, 2, ENC_BIG_ENDIAN);

    /* CmCH-PI */
    proto_tree_add_item(tree, hf_fp_cmch_pi, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* 5 spare bits follow here */

    /* Max MAC-d/c PDU length (11 bits) */
    max_pdu_length = tvb_get_ntohs(tvb, offset) & 0x7ff;
    proto_tree_add_item(tree, hf_fp_hsdsch_max_macdc_pdu_len, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    /* HS-DSCH credits (16 bits) */
    credits = (tvb_get_ntohs(tvb, offset));
    ti = proto_tree_add_uint(tree, hf_fp_hsdsch_credits, tvb,
                             offset, 2, credits);
    offset += 2;

    /* Interesting values */
    if (credits == 0) {
        proto_item_append_text(ti, " (stop transmission)");
        expert_add_info(pinfo, ti, &ei_fp_stop_hsdpa_transmission);
    }
    if (credits == 65535) {
        proto_item_append_text(ti, " (unlimited)");
    }

    /* HS-DSCH Interval */
    interval = tvb_get_guint8(tvb, offset);
    ti = proto_tree_add_uint(tree, hf_fp_hsdsch_interval, tvb, offset, 1, interval*10);
    offset++;
    if (interval == 0) {
        proto_item_append_text(ti, " (none of the credits shall be used)");
    }

    /* HS-DSCH Repetition period */
    repetition_period = tvb_get_guint8(tvb, offset);
    ti = proto_tree_add_item(tree, hf_fp_hsdsch_repetition_period, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;
    if (repetition_period == 0) {
        proto_item_append_text(ti, " (unlimited repetition period)");
    }

    /* Calculated and show effective rate enabled */
    if (credits == 65535) {
        rate_ti = proto_tree_add_item(tree, hf_fp_hsdsch_unlimited_rate, tvb, 0, 0, ENC_NA);
        PROTO_ITEM_SET_GENERATED(rate_ti);
    }
    else {
        if (interval != 0) {
            rate_ti = proto_tree_add_uint(tree, hf_fp_hsdsch_calculated_rate, tvb, 0, 0,
                                          credits * max_pdu_length * (1000 / (interval*10)));
            PROTO_ITEM_SET_GENERATED(rate_ti);
        }
    }

    col_append_fstr(pinfo->cinfo, COL_INFO,
                    "   Max-PDU-len=%u  Credits=%u  Interval=%u  Rep-Period=%u",
                    max_pdu_length, credits, interval, repetition_period);

    return offset;
}
