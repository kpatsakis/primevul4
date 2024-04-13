static char *r_bin_wasm_type_entry_to_string (RBinWasmTypeEntry *ptr) {
	if (!ptr || ptr->to_str) {
		return NULL;
	}

	char *ret;

	int p, i = 0, sz;

	sz = (ptr->param_count + ptr->return_count) * 5 + 9;

	if (!(ret = (char*) malloc (sz * sizeof(char)))) {
		return NULL;
	}

	strcpy (ret + i, "(");
	i++;

	for (p = 0; p < ptr->param_count; p++ ) {
		R_BIN_WASM_VALUETYPETOSTRING (ret+i, ptr->param_types[p], i); // i+=3
		if (p < ptr->param_count - 1) {
			strcpy (ret+i, ", ");
			i += 2;
		}
	}		

	strcpy (ret + i, ") -> (");
	i += 6;

	if (ptr->return_count == 1) {
		R_BIN_WASM_VALUETYPETOSTRING (ret + i, ptr->return_type, i);
	}

	strcpy (ret + i, ")");

	return ret;
}
