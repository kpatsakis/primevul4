dissect_macd_pdu_data(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                      int offset, guint16 length, guint16 number_of_pdus,
                      struct fp_info *p_fp_info, void *data)
{
    int         pdu;
    int         bit_offset = 0;
    proto_item *pdus_ti    = NULL;
    proto_tree *data_tree  = NULL;
    gboolean    dissected  = FALSE;

    /* Add data subtree */
    if (tree) {
        pdus_ti =  proto_tree_add_item(tree, hf_fp_data, tvb, offset, -1, ENC_NA);
        proto_item_set_text(pdus_ti, "%u MAC-d PDUs of %u bits", number_of_pdus, length);
        data_tree = proto_item_add_subtree(pdus_ti, ett_fp_data);
    }

    /* Now for the PDUs */
    for (pdu=0; pdu < number_of_pdus; pdu++) {
        proto_item *pdu_ti;

        if (data_tree) {
            /* Show 4 bits padding at start of PDU */
            proto_tree_add_item(data_tree, hf_fp_hsdsch_data_padding, tvb, offset+(bit_offset/8), 1, ENC_BIG_ENDIAN);

        }
        bit_offset += 4;

        /* Data bytes! */
        if (data_tree) {
            pinfo->fd->subnum = pdu; /* set subframe number to current TB */
            p_fp_info->cur_tb = pdu;    /*Set TB (PDU) index correctly*/
            pdu_ti = proto_tree_add_item(data_tree, hf_fp_mac_d_pdu, tvb,
                                         offset + (bit_offset/8),
                                         ((bit_offset % 8) + length + 7) / 8,
                                         ENC_NA);
            proto_item_set_text(pdu_ti, "MAC-d PDU (PDU %u)", pdu+1);
        }
        if (preferences_call_mac_dissectors /*&& !rlc_is_ciphered(pinfo)*/) {
            tvbuff_t *next_tvb;
            next_tvb = tvb_new_subset(tvb, offset + bit_offset/8,
                                      ((bit_offset % 8) + length + 7)/8, -1);
            call_dissector_with_data(mac_fdd_hsdsch_handle, next_tvb, pinfo, top_level_tree, data);
            dissected = TRUE;
        }

        /* Advance bit offset */
        bit_offset += length;

        /* Pad out to next byte */
        if (bit_offset % 8) {
            bit_offset += (8 - (bit_offset % 8));
        }
    }

    /* Data tree should cover entire length */
    proto_item_set_len(pdus_ti, bit_offset/8);

    /* Move offset past PDUs (we know it's already padded out to next byte) */
    offset += (bit_offset / 8);

    /* Show summary in info column */
    if (dissected == FALSE) {
        col_append_fstr(pinfo->cinfo, COL_INFO, "   %u PDUs of %u bits",
                        number_of_pdus, length);
    }

    return offset;
}
