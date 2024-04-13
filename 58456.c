RList *r_bin_wasm_get_sections (RBinWasmObj *bin) {
	RList *ret = NULL;
	RBinWasmSection *ptr = NULL;

	if (!bin) {
		return NULL;
	}
	if (bin->g_sections) {
		return bin->g_sections;
	}

	if (!(ret = r_list_newf ((RListFree)free))) {
		return NULL;
	}

	ut8* buf = bin->buf->buf;
	ut32 len = bin->size, i = 8; // skip magic bytes + version

	while (i < len) {

		if (!(ptr = R_NEW0 (RBinWasmSection))) {
			return ret;
		}

		if (!(consume_u8 (buf + i, buf + len, &ptr->id, &i))) {
			return ret;
		}

		if (!(consume_u32 (buf + i, buf + len, &ptr->size, &i))) {
			free(ptr);
			return NULL;
		}	

		ptr->count = 0;
		ptr->offset = i;

		switch (ptr->id) {

		case R_BIN_WASM_SECTION_CUSTOM:
			if (!(consume_u32 (buf + i, buf + len, &ptr->name_len, &i))) {
				free(ptr);
				return ret;
			}
			if (!(consume_str (buf + i, buf + len, ptr->name_len,
					ptr->name, &i))) {
				free(ptr);
				return ret;
			}
			break;

		case R_BIN_WASM_SECTION_TYPE:
			strcpy (ptr->name, "type");
			ptr->name_len = 4;
			break;

		case R_BIN_WASM_SECTION_IMPORT:
			strcpy (ptr->name, "import");
			ptr->name_len = 6;
			break;

		case R_BIN_WASM_SECTION_FUNCTION:
			strcpy (ptr->name, "function");
			ptr->name_len = 8;
			break;

		case R_BIN_WASM_SECTION_TABLE:
			strcpy (ptr->name, "table");
			ptr->name_len = 5;
			break;

		case R_BIN_WASM_SECTION_MEMORY:
			strcpy (ptr->name, "memory");
			ptr->name_len = 6;
			break;

		case R_BIN_WASM_SECTION_GLOBAL:
			strcpy (ptr->name, "global");
			ptr->name_len = 6;
			break;

		case R_BIN_WASM_SECTION_EXPORT:
			strcpy (ptr->name, "export");
			ptr->name_len = 6;
			break;

		case R_BIN_WASM_SECTION_START:
			strcpy (ptr->name, "start");
			ptr->name_len = 5;
			break;

		case R_BIN_WASM_SECTION_ELEMENT:
			strncpy (ptr->name, "element", R_BIN_WASM_STRING_LENGTH);
			ptr->name_len = 7;
			break;

		case R_BIN_WASM_SECTION_CODE:
			strncpy (ptr->name, "code", R_BIN_WASM_STRING_LENGTH);
			ptr->name_len = 4;
			break;

		case R_BIN_WASM_SECTION_DATA:
			strncpy (ptr->name, "data", R_BIN_WASM_STRING_LENGTH);
			ptr->name_len = 4;
			break;

		default:
			eprintf("unkown section id: %d\n", ptr->id);
			i += ptr->size - 1; // next
			continue;

		}

		if (ptr->id != R_BIN_WASM_SECTION_START
				&& ptr->id != R_BIN_WASM_SECTION_CUSTOM) {
			if (!(consume_u32 (buf + i, buf + len, &ptr->count, &i))) {
				free (ptr);
				return ret;
			}
		}

		ptr->payload_data = i;
		ptr->payload_len = ptr->size - (i - ptr->offset);

		r_list_append (ret, ptr);

		i += ptr->payload_len; // next

	}

	bin->g_sections = ret;

	return ret;

}
