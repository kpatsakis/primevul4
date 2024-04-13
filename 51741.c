dissect_common_outer_loop_power_control(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb,
                                        int offset, struct fp_info *p_fp_info _U_)
{
    return dissect_dch_outer_loop_power_control(tree, pinfo, tvb, offset);
}
