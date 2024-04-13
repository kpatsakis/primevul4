dissect_dch_outer_loop_power_control(proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb, int offset)
{
    /* UL SIR target */
    float target = (float)-8.2 + ((float)0.1 * (float)(int)(tvb_get_guint8(tvb, offset)));
    proto_tree_add_float(tree, hf_fp_ul_sir_target, tvb, offset, 1, target);
    offset++;

    col_append_fstr(pinfo->cinfo, COL_INFO, " UL SIR Target = %f", target);

    return offset;
}
