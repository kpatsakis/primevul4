int wasm_asm(const char *str, unsigned char *buf, int buf_len) {
	int i = 0, len = -1;
	char tmp[R_ASM_BUFSIZE];
	while (str[i] != ' ' && i < buf_len) {
		tmp[i] = str[i];
		i++;
	}
	tmp[i] = 0;
	for (i = 0; i < 0xff; i++) {
		WasmOpDef *opdef = &opcodes[i];
		if (opdef->txt) {
			if (!strcmp (opdef->txt, tmp)) {
				buf[0] = i;
				return 1;
			}
		}
	}
	return len;
}
