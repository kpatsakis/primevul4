dissect_dch_radio_interface_parameter_update(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int offset)
{
    int    n;
    guint8 value;

    /* Show defined flags in these 2 bytes */
    for (n=4; n >= 0; n--) {
        proto_tree_add_item(tree, hf_fp_radio_interface_parameter_update_flag[n], tvb, offset, 2, ENC_BIG_ENDIAN);
    }
    offset += 2;

    /* CFN  */
    tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* DPC mode */
    proto_tree_add_item(tree, hf_fp_dpc_mode, tvb, offset, 1, ENC_BIG_ENDIAN);

    /* TPC PO */
    proto_tree_add_item(tree, hf_fp_tpc_po, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* Multiple RL sets indicator */
    proto_tree_add_item(tree, hf_fp_multiple_rl_set_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 2;

    /* MAX_UE_TX_POW */
    value = (tvb_get_guint8(tvb, offset) & 0x7f);
    proto_tree_add_int(tree, hf_fp_max_ue_tx_pow, tvb, offset, 1, -55 + value);
    offset++;

    return offset;
}
