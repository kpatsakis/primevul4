dissect_header_lens_v2_large(tvbuff_t *tvb, wtap_syscall_header* syscall_header, int offset, proto_tree *tree, int encoding)
{
    guint32 param_count;
    proto_item *ti;
    proto_tree *len_tree;

    ti = proto_tree_add_item(tree, hf_se_param_lens, tvb, offset, syscall_header->nparams * SYSDIG_PARAM_SIZE_V2_LARGE, ENC_NA);
    len_tree = proto_item_add_subtree(ti, ett_sysdig_parm_lens);

    for (param_count = 0; param_count < syscall_header->nparams; param_count++) {
        proto_tree_add_item(len_tree, hf_se_param_len, tvb, offset + (param_count * SYSDIG_PARAM_SIZE_V2_LARGE), SYSDIG_PARAM_SIZE_V2_LARGE, encoding);
    }

    proto_item_set_len(ti, syscall_header->nparams * SYSDIG_PARAM_SIZE_V2_LARGE);
    return syscall_header->nparams * SYSDIG_PARAM_SIZE_V2_LARGE;
}