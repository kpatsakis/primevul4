static int dalvik_assemble(RAsm *a, RAsmOp *op, const char *buf) {
	int i;
	char *p = strchr (buf, ' ');
	if (p) {
		*p = 0;
	}
	for (i = 0; i < 256; i++) {
		if (!strcmp (dalvik_opcodes[i].name, buf)) {
			r_write_ble32 (op->buf, i, a->big_endian);
			op->size = dalvik_opcodes[i].len;
			return op->size;
		}
	}
	return 0;
}
