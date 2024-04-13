static const ut8 *parse_dex_class_fields(RBinFile *binfile, RBinDexObj *bin,
					  RBinDexClass *c, RBinClass *cls,
					  const ut8 *p, const ut8 *p_end,
					  int *sym_count, ut64 fields_count,
					  bool is_sfield) {
	struct r_bin_t *rbin = binfile->rbin;
	ut64 lastIndex = 0;
	ut8 ff[sizeof (DexField)] = {0};
	int total, i, tid;
	DexField field;
	const char* type_str;
	for (i = 0; i < fields_count; i++) {
		ut64 fieldIndex, accessFlags;
		
		p = r_uleb128 (p, p_end - p, &fieldIndex); // fieldIndex
		p = r_uleb128 (p, p_end - p, &accessFlags); // accessFlags
		fieldIndex += lastIndex;
		total = bin->header.fields_offset + (sizeof (DexField) * fieldIndex);
		if (total >= bin->size || total < bin->header.fields_offset) {
			break;	
		}
		if (r_buf_read_at (binfile->buf, total, ff,
				sizeof (DexField)) != sizeof (DexField)) {
			break;
		}
		field.class_id = r_read_le16 (ff);
		field.type_id = r_read_le16 (ff + 2);
		field.name_id = r_read_le32 (ff + 4);
		char *fieldName = getstr (bin, field.name_id);
		if (field.type_id >= bin->header.types_size) {
			break;
		}
		tid = bin->types[field.type_id].descriptor_id;
		type_str = getstr (bin, tid);
		RBinSymbol *sym = R_NEW0 (RBinSymbol);
		if (is_sfield) {
			sym->name = r_str_newf ("%s.sfield_%s:%s", cls->name,
						fieldName, type_str);
			sym->type = r_str_const ("STATIC");
		} else {
			sym->name = r_str_newf ("%s.ifield_%s:%s", cls->name,
						fieldName, type_str);
			sym->type = r_str_const ("FIELD");
		}
		sym->name = r_str_replace (sym->name, "method.", "", 0);
		sym->paddr = sym->vaddr = total;
		sym->ordinal = (*sym_count)++;
		
		if (dexdump) {
			const char *accessStr = createAccessFlagStr (
				accessFlags, kAccessForField);
			rbin->cb_printf ("    #%d              : (in %s;)\n", i,
					 cls->name);
			rbin->cb_printf ("      name          : '%s'\n", fieldName);
			rbin->cb_printf ("      type          : '%s'\n", type_str);
			rbin->cb_printf ("      access        : 0x%04x (%s)\n",
					 (unsigned int)accessFlags, accessStr);
		}

		r_list_append (bin->methods_list, sym);
		r_list_append (cls->fields, sym);

		lastIndex = fieldIndex;
	}
	return p;
}
