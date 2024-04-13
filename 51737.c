dissect_common_control(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                       int offset, struct fp_info *p_fp_info)
{
    /* Common control frame type */
    guint8 control_frame_type = tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_common_control_frame_type, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO,
                   val_to_str_const(control_frame_type, common_control_frame_type_vals, "Unknown"));

    /* Frame-type specific dissection */
    switch (control_frame_type) {
        case COMMON_OUTER_LOOP_POWER_CONTROL:
            /*offset =*/ dissect_common_outer_loop_power_control(pinfo, tree, tvb, offset, p_fp_info);
            break;
        case COMMON_TIMING_ADJUSTMENT:
            /*offset =*/ dissect_common_timing_adjustment(pinfo, tree, tvb, offset, p_fp_info);
            break;
        case COMMON_DL_SYNCHRONISATION:
            /*offset =*/ dissect_common_dl_synchronisation(pinfo, tree, tvb, offset, p_fp_info);
            break;
        case COMMON_UL_SYNCHRONISATION:
            /*offset =*/ dissect_common_ul_synchronisation(pinfo, tree, tvb, offset, p_fp_info);
            break;
        case COMMON_DL_NODE_SYNCHRONISATION:
            /*offset =*/ dissect_common_dl_node_synchronisation(pinfo, tree, tvb, offset);
            break;
        case COMMON_UL_NODE_SYNCHRONISATION:
            /*offset =*/ dissect_common_ul_node_synchronisation(pinfo, tree, tvb, offset);
            break;
        case COMMON_DYNAMIC_PUSCH_ASSIGNMENT:
            /*offset =*/ dissect_common_dynamic_pusch_assignment(pinfo, tree, tvb, offset);
            break;
        case COMMON_TIMING_ADVANCE:
            /*offset =*/ dissect_common_timing_advance(pinfo, tree, tvb, offset);
            break;
        case COMMON_HS_DSCH_Capacity_Request:
            /*offset =*/ dissect_hsdpa_capacity_request(pinfo, tree, tvb, offset);
            break;
        case COMMON_HS_DSCH_Capacity_Allocation:
            /*offset =*/ dissect_hsdpa_capacity_allocation(pinfo, tree, tvb, offset, p_fp_info);
            break;
        case COMMON_HS_DSCH_Capacity_Allocation_Type_2:
            /*offset =*/ dissect_hsdpa_capacity_allocation_type_2(pinfo, tree, tvb, offset);
            break;

        default:
            break;
    }

     /* There is no Spare Extension nor payload crc in common control!? */
   /* dissect_spare_extension_and_crc(tvb, pinfo, tree, 0, offset);
    */
}
