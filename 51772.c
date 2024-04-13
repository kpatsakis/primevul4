dissect_tb_data(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                int offset, struct fp_info *p_fp_info,
                dissector_handle_t *data_handle, void *data)
{
    int         chan, num_tbs   = 0;
    int         bit_offset      = 0;
    int         crci_bit_offset = (offset+1)<<3; /* Current offset + Quality estimate of 1 byte at the end*/
    guint       data_bits       = 0;
    guint8      crci_bit        = 0;
    proto_item *tree_ti         = NULL;
    proto_tree *data_tree       = NULL;
    gboolean    dissected       = FALSE;

    if (tree) {
        /* Add data subtree */
        tree_ti =  proto_tree_add_item(tree, hf_fp_data, tvb, offset, -1, ENC_NA);
        proto_item_set_text(tree_ti, "TB data for %u chans", p_fp_info->num_chans);
        data_tree = proto_item_add_subtree(tree_ti, ett_fp_data);
    }

    /* Calculate offset to CRCI bits */

    if (p_fp_info->is_uplink) {
        for (chan=0; chan < p_fp_info->num_chans; chan++) {
            int n;
            for (n=0; n < p_fp_info->chan_num_tbs[chan]; n++) {
                /* Advance bit offset */
                crci_bit_offset += p_fp_info->chan_tf_size[chan];
                /* Pad out to next byte */
                if (crci_bit_offset % 8) {
                    crci_bit_offset += (8 - (crci_bit_offset % 8));
                }
            }
        }
    }
    /* Now for the TB data */
    for (chan=0; chan < p_fp_info->num_chans; chan++) {
        int n;
        p_fp_info->cur_chan = chan;    /*Set current channel?*/
        /* Clearly show channels with no TBs */
        if (p_fp_info->chan_num_tbs[chan] == 0) {
            proto_item *no_tb_ti = proto_tree_add_uint(data_tree, hf_fp_chan_zero_tbs, tvb,
                                                       offset+(bit_offset/8),
                                                       0, chan+1);
            proto_item_append_text(no_tb_ti, " (of size %d)",
                                   p_fp_info->chan_tf_size[chan]);
            PROTO_ITEM_SET_GENERATED(no_tb_ti);
        }

        /* Show TBs from non-empty channels */
        pinfo->fd->subnum = chan; /* set subframe number to current TB */
        for (n=0; n < p_fp_info->chan_num_tbs[chan]; n++) {

            proto_item *ti;
            p_fp_info->cur_tb = chan;    /*Set current transport block?*/
            if (data_tree) {
                ti = proto_tree_add_item(data_tree, hf_fp_tb, tvb,
                                         offset + (bit_offset/8),
                                         ((bit_offset % 8) + p_fp_info->chan_tf_size[chan] + 7) / 8,
                                         ENC_NA);
                proto_item_set_text(ti, "TB (chan %u, tb %u, %u bits)",
                                    chan+1, n+1, p_fp_info->chan_tf_size[chan]);
            }

            if (preferences_call_mac_dissectors /*&& !rlc_is_ciphered(pinfo)*/ && data_handle &&
                (p_fp_info->chan_tf_size[chan] > 0)) {
                tvbuff_t *next_tvb;
                proto_item *item;
                /* If this is DL we should not care about crci bits (since they don't exists)*/
                if (p_fp_info->is_uplink) {


                    if ( p_fp_info->channel == CHANNEL_RACH_FDD) {    /*In RACH we don't have any QE field, hence go back 8 bits.*/
                        crci_bit = tvb_get_bits8(tvb, crci_bit_offset+n-8, 1);
                        item = proto_tree_add_item(data_tree, hf_fp_crci[n%8], tvb, (crci_bit_offset+n-8)/8, 1, ENC_BIG_ENDIAN);
                        PROTO_ITEM_SET_GENERATED(item);
                    } else {
                        crci_bit = tvb_get_bits8(tvb, crci_bit_offset+n, 1);
                        item = proto_tree_add_item(data_tree, hf_fp_crci[n%8], tvb, (crci_bit_offset+n)/8, 1, ENC_BIG_ENDIAN);
                        PROTO_ITEM_SET_GENERATED(item);
                    }
                }

                if (crci_bit == 0 || !p_fp_info->is_uplink) {
                    next_tvb = tvb_new_subset(tvb, offset + bit_offset/8,
                                              ((bit_offset % 8) + p_fp_info->chan_tf_size[chan] + 7) / 8, -1);


                    /****************/
                    /* TODO: maybe this decision can be based only on info available in fp_info */
                    call_dissector_with_data(*data_handle, next_tvb, pinfo, top_level_tree, data);
                    dissected = TRUE;
                } else {
                    proto_tree_add_expert(tree, pinfo, &ei_fp_crci_no_subdissector, tvb, offset + bit_offset/8,
                                               ((bit_offset % 8) + p_fp_info->chan_tf_size[chan] + 7) / 8);
                }

            }
            num_tbs++;

            /* Advance bit offset */
            bit_offset += p_fp_info->chan_tf_size[chan];
            data_bits  += p_fp_info->chan_tf_size[chan];

            /* Pad out to next byte */
            if (bit_offset % 8) {
                bit_offset += (8 - (bit_offset % 8));
            }
        }
    }

    if (dissected == FALSE) {
        col_append_fstr(pinfo->cinfo, COL_INFO, "(%u bits in %u tbs)",
                        data_bits, num_tbs);
    }

    /* Data tree should cover entire length */
    if (data_tree) {
        proto_item_set_len(tree_ti, bit_offset/8);
        proto_item_append_text(tree_ti, " (%u bits in %u tbs)", data_bits, num_tbs);
    }

    /* Move offset past TBs (we know it's already padded out to next byte) */
    offset += (bit_offset / 8);

    return offset;
}
