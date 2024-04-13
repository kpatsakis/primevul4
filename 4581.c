GF_Err gf_isom_parse_root_box(GF_Box **outBox, GF_BitStream *bs, u32 *box_type, u64 *bytesExpected, Bool progressive_mode)
{
	GF_Err ret;
	u64 start;
	start = gf_bs_get_position(bs);
	ret = gf_isom_box_parse_ex(outBox, bs, 0, GF_TRUE, 0);
	if (ret == GF_ISOM_INCOMPLETE_FILE) {
		if (!*outBox) {
			// We could not even read the box size, we at least need 8 bytes
			*bytesExpected = 8;
			if (box_type) *box_type = 0;
			GF_LOG(progressive_mode ? GF_LOG_DEBUG : GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Incomplete box - start "LLU"\n", start));
		}
		else {
			u32 type = (*outBox)->type;
			if (type==GF_ISOM_BOX_TYPE_UNKNOWN)
				type = ((GF_UnknownBox *) (*outBox))->original_4cc;

			*bytesExpected = (*outBox)->size;
			if (box_type) *box_type = (*outBox)->type;

			GF_LOG(progressive_mode ? GF_LOG_DEBUG : GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Incomplete box %s - start "LLU" size "LLU"\n", gf_4cc_to_str(type), start, (*outBox)->size));
			gf_isom_box_del(*outBox);
			*outBox = NULL;
		}
		gf_bs_seek(bs, start);
	}
	return ret;
}