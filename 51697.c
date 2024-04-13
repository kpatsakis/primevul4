add_ppi_field_header(tvbuff_t *tvb, proto_tree *tree, int *offset)
{
    ptvcursor_t *csr;

    csr = ptvcursor_new(tree, tvb, *offset);
    ptvcursor_add(csr, hf_ppi_field_type, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_ppi_field_len, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_free(csr);
    *offset=ptvcursor_current_offset(csr);
}
