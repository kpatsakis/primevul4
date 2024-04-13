static const ut8 *parse_dex_class_method(RBinFile *binfile, RBinDexObj *bin,
					  RBinDexClass *c, RBinClass *cls,
					  const ut8 *p, const ut8 *p_end,
					  int *sym_count, ut64 DM, int *methods,
					  bool is_direct) {
	struct r_bin_t *rbin = binfile->rbin;
	ut8 ff2[16] = {0};
	ut8 ff3[8] = {0};
	int i;
	ut64 omi = 0;
	bool catchAll;
	ut16 regsz, ins_size, outs_size, tries_size;
	ut16 handler_off, start_addr, insn_count;
	ut32 debug_info_off, insns_size;
	const ut8 *encoded_method_addr;
	for (i = 0; i < DM; i++) {
		encoded_method_addr = p;
		char *method_name, *flag_name;
		ut64 MI, MA, MC;
		p = r_uleb128 (p, p_end - p, &MI);
		MI += omi;
		omi = MI;
		p = r_uleb128 (p, p_end - p, &MA);
		p = r_uleb128 (p, p_end - p, &MC);
		if (MI < bin->header.method_size) {
			if (methods) {
				methods[MI] = 1;
			}
		}
		method_name = dex_method_name (bin, MI);
		char *signature = dex_method_signature (bin, MI);
		if (!method_name) {
			method_name = strdup ("unknown");
		}
		flag_name = r_str_newf ("%s.method.%s%s", cls->name,
					method_name, signature);
		if (!flag_name) {
			R_FREE (method_name);
			R_FREE (signature);
			continue;
		}
		ut64 v2, handler_type, handler_addr;
		int t;
		if (MC > 0) { 
			if (MC + 16 >= bin->size || MC + 16 < MC) {
				R_FREE (method_name);
				R_FREE (flag_name);
				R_FREE (signature);
				continue;
			}	
			if (r_buf_read_at (binfile->buf,
					   binfile->buf->base + MC, ff2,
					   16) < 1) {
				R_FREE (method_name);
				R_FREE (flag_name);
				R_FREE (signature);
				continue;
			}
			regsz = r_read_le16 (ff2);
			ins_size = r_read_le16 (ff2 + 2);
			outs_size = r_read_le16 (ff2 + 4);
			tries_size = r_read_le16 (ff2 + 6);
			debug_info_off = r_read_le32 (ff2 + 8);
			insns_size = r_read_le32 (ff2 + 12);
			int padd = 0;
			if (tries_size > 0 && insns_size % 2) {
				padd = 2;
			}
			t = 16 + 2 * insns_size + padd;
		}
		if (dexdump) {
			const char* accessStr = createAccessFlagStr (MA, kAccessForMethod);
			rbin->cb_printf ("    #%d              : (in %s;)\n", i, cls->name);
			rbin->cb_printf ("      name          : '%s'\n", method_name);
			rbin->cb_printf ("      type          : '%s'\n", signature);
			rbin->cb_printf ("      access        : 0x%04x (%s)\n",
					 (unsigned int)MA, accessStr);
		}

		if (MC > 0) {
			if (dexdump) {
				rbin->cb_printf ("      code          -\n");
				rbin->cb_printf ("      registers     : %d\n", regsz);
				rbin->cb_printf ("      ins           : %d\n", ins_size);
				rbin->cb_printf ("      outs          : %d\n", outs_size);
				rbin->cb_printf (
					"      insns size    : %d 16-bit code "
					"units\n",
					insns_size);
			}
			if (tries_size > 0) {
				if (dexdump) {
					rbin->cb_printf ("      catches       : %d\n", tries_size);
				}
				int j, m = 0;
				for (j = 0; j < tries_size; ++j) {
					ut64 offset = MC + t + j * 8;
					if (offset >= bin->size || offset < MC) {
						R_FREE (signature);
						break;
					}
					if (r_buf_read_at (
						    binfile->buf,
						    binfile->buf->base + offset,
						    ff3, 8) < 1) {
						R_FREE (signature);
						break;
					}
					start_addr = r_read_le32 (ff3);
					insn_count = r_read_le16 (ff3 + 4);
					handler_off = r_read_le16 (ff3 + 6);
					char* s = NULL;
					if (dexdump) {
						rbin->cb_printf (
							"        0x%04x - "
							"0x%04x\n",
							start_addr,
							(start_addr +
							 insn_count));
					}
					
					const ut8 *p3, *p3_end;
					int off = MC + t + tries_size * 8 + handler_off;
					if (off >= bin->size || off < tries_size) {
						R_FREE (signature);
						break;
					}
					p3 = r_buf_get_at (binfile->buf, off, NULL);
					p3_end = p3 + binfile->buf->length - off;
					st64 size = r_sleb128 (&p3, p3_end);

					if (size <= 0) {
						catchAll = true;
						size = -size;
					} else {
						catchAll = false;
					}

					for (m = 0; m < size; m++) {
						p3 = r_uleb128 (p3, p3_end - p3, &handler_type);
						p3 = r_uleb128 (p3, p3_end - p3, &handler_addr);

						if (handler_type > 0 &&
						    handler_type <
							    bin->header.types_size) {
							s = getstr (bin, bin->types[handler_type].descriptor_id);
							if (dexdump) {
								rbin->cb_printf (
									"          %s "
									"-> 0x%04llx\n",
									s,
									handler_addr);
							}
						} else {
							if (dexdump) {
								rbin->cb_printf (
									"          "
									"(error) -> "
									"0x%04llx\n",
									handler_addr);
							}
						}
					}
					if (catchAll) {
						p3 = r_uleb128 (p3, p3_end - p3, &v2);
						if (dexdump) {
							rbin->cb_printf (
								"          "
								"<any> -> "
								"0x%04llx\n",
								v2);
						}
					}
				}
			} else {
				if (dexdump) {
					rbin->cb_printf (
						"      catches       : "
						"(none)\n");
				}
			}
		} else {
			if (dexdump) {
				rbin->cb_printf (
					"      code          : (none)\n");
			}
		}
		if (*flag_name) {
			RBinSymbol *sym = R_NEW0 (RBinSymbol);
			sym->name = flag_name;
			if (MC > 0) { 
				sym->type = r_str_const ("FUNC");
				sym->paddr = MC;// + 0x10;
				sym->vaddr = MC;// + 0x10;
			} else {
				sym->type = r_str_const ("METH");
				sym->paddr = encoded_method_addr - binfile->buf->buf;
				sym->vaddr = encoded_method_addr - binfile->buf->buf;
			}
			if ((MA & 0x1) == 0x1) {
				sym->bind = r_str_const ("GLOBAL");
			} else {
				sym->bind = r_str_const ("LOCAL");
			}
			sym->ordinal = (*sym_count)++;
			if (MC > 0) {
				if (r_buf_read_at (binfile->buf, binfile->buf->base + MC, ff2, 16) < 1) {
					R_FREE (sym);
					R_FREE (signature);
					continue;
				}
				ut16 tries_size = r_read_le16 (ff2 + 6);
				ut32 insns_size = r_read_le32 (ff2 + 12);
				ut64 prolog_size = 2 + 2 + 2 + 2 + 4 + 4;
				if (tries_size > 0) {
				}
				sym->paddr = MC + prolog_size;// + 0x10;
				sym->vaddr = MC + prolog_size;// + 0x10;
				sym->size = insns_size * 2;
				r_list_append (bin->methods_list, sym);
				r_list_append (cls->methods, sym);

				if (bin->code_from > sym->paddr) {
					bin->code_from = sym->paddr;
				}
				if (bin->code_to < sym->paddr) {
					bin->code_to = sym->paddr;
				}

				if (!mdb) {
					mdb = sdb_new0 ();
				}
				sdb_num_set (mdb, sdb_fmt (0, "method.%d", MI), sym->paddr, 0);
				if (0) {
					if (MA & 0x10000) { //ACC_CONSTRUCTOR
						if (!cdb) {
							cdb = sdb_new0 ();
						}
						sdb_num_set (cdb, sdb_fmt (0, "%d", c->class_id), sym->paddr, 0);
					}
				}
			} else {
				sym->size = 0;
				r_list_append (bin->methods_list, sym);
				r_list_append (cls->methods, sym);
			}
			if (MC > 0 && debug_info_off > 0 && bin->header.data_offset < debug_info_off &&
				debug_info_off < bin->header.data_offset + bin->header.data_size) {
				dex_parse_debug_item (binfile, bin, c, MI, MA, sym->paddr, ins_size, 
					insns_size, cls->name, regsz, debug_info_off);
			} else if (MC > 0) {
				if (dexdump) {
					rbin->cb_printf ("      positions     :\n");
					rbin->cb_printf ("      locals        :\n");
				}
			}
		} else {
			R_FREE (flag_name);
		}
		R_FREE (signature);
		R_FREE (method_name);
	}
	return p;
}
