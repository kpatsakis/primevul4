dissect_rpcap_filterbpf_insn (tvbuff_t *tvb, packet_info *pinfo _U_,
                              proto_tree *parent_tree, gint offset)
{
  proto_tree *tree, *code_tree;
  proto_item *ti, *code_ti;
  guint8 inst_class;

  ti = proto_tree_add_item (parent_tree, hf_filterbpf_insn, tvb, offset, 8, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_filterbpf_insn);

  code_ti = proto_tree_add_item (tree, hf_code, tvb, offset, 2, ENC_BIG_ENDIAN);
  code_tree = proto_item_add_subtree (code_ti, ett_filterbpf_insn_code);
  proto_tree_add_item (code_tree, hf_code_class, tvb, offset, 2, ENC_BIG_ENDIAN);
  inst_class = tvb_get_guint8 (tvb, offset + 1) & 0x07;
  proto_item_append_text (ti, ": %s", val_to_str_const (inst_class, bpf_class, ""));
  switch (inst_class) {
  case 0x00: /* ld */
  case 0x01: /* ldx */
    proto_tree_add_item (code_tree, hf_code_ld_size, tvb, offset, 2, ENC_BIG_ENDIAN);
    proto_tree_add_item (code_tree, hf_code_ld_mode, tvb, offset, 2, ENC_BIG_ENDIAN);
    break;
  case 0x04: /* alu */
    proto_tree_add_item (code_tree, hf_code_src, tvb, offset, 2, ENC_BIG_ENDIAN);
    proto_tree_add_item (code_tree, hf_code_alu_op, tvb, offset, 2, ENC_BIG_ENDIAN);
    break;
  case 0x05: /* jmp */
    proto_tree_add_item (code_tree, hf_code_src, tvb, offset, 2, ENC_BIG_ENDIAN);
    proto_tree_add_item (code_tree, hf_code_jmp_op, tvb, offset, 2, ENC_BIG_ENDIAN);
    break;
  case 0x06: /* ret */
    proto_tree_add_item (code_tree, hf_code_rval, tvb, offset, 2, ENC_BIG_ENDIAN);
    break;
  case 0x07: /* misc */
    proto_tree_add_item (code_tree, hf_code_misc_op, tvb, offset, 2, ENC_BIG_ENDIAN);
    break;
  default:
    proto_tree_add_item (code_tree, hf_code_fields, tvb, offset, 2, ENC_BIG_ENDIAN);
    break;
  }
  offset += 2;

  proto_tree_add_item (tree, hf_jt, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  proto_tree_add_item (tree, hf_jf, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  proto_tree_add_item (tree, hf_instr_value, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  return offset;
}
