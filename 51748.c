dissect_dch_control_frame(proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb,
                          int offset, struct fp_info *p_fp_info)
{
    /* Control frame type */
    guint8 control_frame_type = tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_dch_control_frame_type, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO,
                   val_to_str_const(control_frame_type,
                                    dch_control_frame_type_vals, "Unknown"));

    switch (control_frame_type) {
        case DCH_TIMING_ADJUSTMENT:
            /*offset =*/ dissect_dch_timing_adjustment(tree, pinfo, tvb, offset);
            break;
        case DCH_RX_TIMING_DEVIATION:
            /*offset =*/ dissect_dch_rx_timing_deviation(pinfo, tree, tvb, offset, p_fp_info);
            break;
        case DCH_DL_SYNCHRONISATION:
            /*offset =*/ dissect_dch_dl_synchronisation(tree, pinfo, tvb, offset);
            break;
        case DCH_UL_SYNCHRONISATION:
            /*offset =*/ dissect_dch_ul_synchronisation(tree, pinfo, tvb, offset);
            break;
        case DCH_OUTER_LOOP_POWER_CONTROL:
            /*offset =*/ dissect_dch_outer_loop_power_control(tree, pinfo, tvb, offset);
            break;
        case DCH_DL_NODE_SYNCHRONISATION:
            /*offset =*/ dissect_dch_dl_node_synchronisation(tree, pinfo, tvb, offset);
            break;
        case DCH_UL_NODE_SYNCHRONISATION:
            /*offset =*/ dissect_dch_ul_node_synchronisation(tree, pinfo, tvb, offset);
            break;
        case DCH_RADIO_INTERFACE_PARAMETER_UPDATE:
            /*offset =*/ dissect_dch_radio_interface_parameter_update(tree, pinfo, tvb, offset);
            break;
        case DCH_TIMING_ADVANCE:
            /*offset =*/ dissect_dch_timing_advance(tree, pinfo, tvb, offset, p_fp_info);
            break;
        case DCH_TNL_CONGESTION_INDICATION:
            /*offset =*/ dissect_dch_tnl_congestion_indication(tree, pinfo, tvb, offset);
            break;
    }

    /* Spare Extension */
   /* dissect_spare_extension_and_crc(tvb, pinfo, tree, 0, offset);
    */
}
