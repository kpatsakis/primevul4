dissect_macd_pdu_data_type_2(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                             int offset, guint16 length, guint16 number_of_pdus,
                             struct fp_info *fpi, void *data)
{
    int         pdu;
    proto_item *pdus_ti      = NULL;
    proto_tree *data_tree    = NULL;
    int         first_offset = offset;
    gboolean    dissected    = FALSE;

    /* Add data subtree */
    if (tree) {
        pdus_ti =  proto_tree_add_item(tree, hf_fp_data, tvb, offset, -1, ENC_NA);
        proto_item_set_text(pdus_ti, "%u MAC-d PDUs of %u bytes", number_of_pdus, length);
        data_tree = proto_item_add_subtree(pdus_ti, ett_fp_data);
    }

    /* Now for the PDUs */
    for (pdu=0; pdu < number_of_pdus; pdu++) {
        proto_item *pdu_ti;

        /* Data bytes! */
        if (data_tree) {
            pdu_ti = proto_tree_add_item(data_tree, hf_fp_mac_d_pdu, tvb,
                                         offset, length, ENC_NA);
            proto_item_set_text(pdu_ti, "MAC-d PDU (PDU %u)", pdu+1);

        }

        if (preferences_call_mac_dissectors  /*&& !rlc_is_ciphered(pinfo)*/) {

            tvbuff_t *next_tvb = tvb_new_subset(tvb, offset, length, -1);


            fpi->cur_tb = pdu;    /*Set proper pdu index for MAC and higher layers*/
            call_dissector_with_data(mac_fdd_hsdsch_handle, next_tvb, pinfo, top_level_tree, data);
            dissected = TRUE;
        }

        /* Advance offset */
        offset += length;
    }

    /* Data tree should cover entire length */
    proto_item_set_len(pdus_ti, offset-first_offset);

    /* Show summary in info column */
    if (!dissected) {
        col_append_fstr(pinfo->cinfo, COL_INFO, "   %u PDUs of %u bits",
                        number_of_pdus, length*8);
    }

    return offset;
}
