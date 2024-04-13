static RList *strings(RBinFile *arch) {
	struct r_bin_dex_obj_t *bin = NULL;
	RBinString *ptr = NULL;
	RList *ret = NULL;
	int i, len;
	ut8 buf[6];
	ut64 off;
	if (!arch || !arch->o) {
		return NULL;
	}
	bin = (struct r_bin_dex_obj_t *) arch->o->bin_obj;
	if (!bin || !bin->strings) {
		return NULL;
	}
	if (bin->header.strings_size > bin->size) {
		bin->strings = NULL;
		return NULL;
	}
	if (!(ret = r_list_newf (free))) {
		return NULL;
	}
	for (i = 0; i < bin->header.strings_size; i++) {
		if (!(ptr = R_NEW0 (RBinString))) {
			break;
		}
		if (bin->strings[i] > bin->size || bin->strings[i] + 6 > bin->size) {
			goto out_error;
		}
		r_buf_read_at (bin->b, bin->strings[i], (ut8*)&buf, 6);
		len = dex_read_uleb128 (buf);

		if (len > 1 && len < R_BIN_SIZEOF_STRINGS) {
			ptr->string = malloc (len + 1);
			if (!ptr->string) {
				goto out_error;
			}
			off = bin->strings[i] + dex_uleb128_len (buf);
			if (off + len >= bin->size || off + len < len) {
				free (ptr->string);
				goto out_error;
			}
			r_buf_read_at (bin->b, off, (ut8*)ptr->string, len);
			ptr->string[len] = 0;
			ptr->vaddr = ptr->paddr = bin->strings[i];
			ptr->size = len;
			ptr->length = len;
			ptr->ordinal = i+1;
			r_list_append (ret, ptr);
		} else {
			free (ptr);
		}
	}
	return ret;
out_error:
	r_list_free (ret);
	free (ptr);
	return NULL;
}
