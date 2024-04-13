dissect_header_lens_v1(tvbuff_t *tvb, int offset, proto_tree *tree, int encoding, int * const *hf_indexes)
{
    int param_count;
    proto_item *ti;
    proto_tree *len_tree;

    for (param_count = 0; hf_indexes[param_count]; param_count++);

    ti = proto_tree_add_item(tree, hf_se_param_lens, tvb, offset, param_count * SYSDIG_PARAM_SIZE, ENC_NA);
    len_tree = proto_item_add_subtree(ti, ett_sysdig_parm_lens);

    for (param_count = 0; hf_indexes[param_count]; param_count++) {
        proto_tree_add_item(len_tree, hf_se_param_len, tvb, offset + (param_count * SYSDIG_PARAM_SIZE), SYSDIG_PARAM_SIZE, encoding);
    }

    proto_item_set_len(ti, param_count * SYSDIG_PARAM_SIZE);
    return param_count * SYSDIG_PARAM_SIZE;
}