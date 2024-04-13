dissect_common_ul_synchronisation(packet_info *pinfo, proto_tree *tree,
                                  tvbuff_t *tvb, int offset, struct fp_info *p_fp_info)
{
    return dissect_common_timing_adjustment(pinfo, tree, tvb, offset, p_fp_info);
}
