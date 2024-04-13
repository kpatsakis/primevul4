dissect_8023_extension(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, int offset, int data_len)
{
    proto_tree  *ftree;
    ptvcursor_t *csr;

    ftree = proto_tree_add_subtree(tree, tvb, offset, data_len, ett_8023_extension, NULL, "802.3 Extension");
    add_ppi_field_header(tvb, ftree, &offset);
    data_len -= 4; /* Subtract field header length */

    if (data_len != PPI_8023_EXTENSION_LEN) {
        proto_tree_add_expert_format(ftree, pinfo, &ei_ppi_invalid_length, tvb, offset, data_len, "Invalid length: %u", data_len);
        THROW(ReportedBoundsError);
    }

    csr = ptvcursor_new(ftree, tvb, offset);

    ptvcursor_add_with_subtree(csr, hf_8023_extension_flags, 4, ENC_LITTLE_ENDIAN, ett_8023_extension_flags);
    ptvcursor_add(csr, hf_8023_extension_flags_fcs_present, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_pop_subtree(csr);

    ptvcursor_add_with_subtree(csr, hf_8023_extension_errors, 4, ENC_LITTLE_ENDIAN, ett_8023_extension_errors);
    ptvcursor_add_no_advance(csr, hf_8023_extension_errors_fcs, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_8023_extension_errors_sequence, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_8023_extension_errors_symbol, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_8023_extension_errors_data, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_pop_subtree(csr);

    ptvcursor_free(csr);
}
