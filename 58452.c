static RList *r_bin_wasm_get_import_entries (RBinWasmObj *bin, RBinWasmSection *sec) {
	RList *ret = NULL;
	RBinWasmImportEntry *ptr = NULL;

	if (!(ret = r_list_newf ((RListFree)free))) {
		return NULL;
	}

	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	ut32 len =  sec->payload_len;
	ut32 count = sec->count;
	ut32 i = 0, r = 0;

	while (i < len && r < count) {
		if (!(ptr = R_NEW0 (RBinWasmImportEntry))) {
			return ret;
		}
		if (!(consume_u32 (buf + i, buf + len, &ptr->module_len, &i))) {
			goto culvert;
		}
		if (!(consume_str (buf + i, buf + len, ptr->module_len, ptr->module_str, &i))) {
			goto culvert;
		}
		if (!(consume_u32 (buf + i, buf + len, &ptr->field_len, &i))) {
			goto culvert;
		}
		if (!(consume_str (buf + i, buf + len, ptr->field_len, ptr->field_str, &i))) {
			goto culvert;
		} 
		if (!(consume_u8 (buf + i, buf + len, &ptr->kind, &i))) {
			goto culvert;
		}
		switch (ptr->kind) {
		case 0: // Function
			if (!(consume_u32 (buf + i, buf + len, &ptr->type_f, &i))) {
				goto sewer;
			}
			break;
		case 1: // Table
			if (!(consume_u8 (buf + i, buf + len, (ut8*)&ptr->type_t.elem_type, &i))) {
				goto sewer; // varint7
			}
			if (!(consume_limits (buf + i, buf + len, &ptr->type_t.limits, &i))) {
				goto sewer;
			}
			break;
		case 2: // Memory
			if (!(consume_limits (buf + i, buf + len, &ptr->type_m.limits, &i))) {
				goto sewer;
			}
			break;
		case 3: // Global
			if (!(consume_u8 (buf + i, buf + len, (ut8*)&ptr->type_g.content_type, &i))) {
				goto sewer; // varint7
			}
			if (!(consume_u8 (buf + i, buf + len, (ut8*)&ptr->type_g.mutability, &i))) {
				goto sewer; // varuint1
			}
			break;
		default:
			goto sewer;
		}
		r_list_append (ret, ptr);
		r++;
	}
	return ret;
sewer:
	ret = NULL;
culvert:
	free (ptr);
	return ret;
}
