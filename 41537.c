static int tg3_test_memory(struct tg3 *tp)
{
	static struct mem_entry {
		u32 offset;
		u32 len;
	} mem_tbl_570x[] = {
		{ 0x00000000, 0x00b50},
		{ 0x00002000, 0x1c000},
		{ 0xffffffff, 0x00000}
	}, mem_tbl_5705[] = {
		{ 0x00000100, 0x0000c},
		{ 0x00000200, 0x00008},
		{ 0x00004000, 0x00800},
		{ 0x00006000, 0x01000},
		{ 0x00008000, 0x02000},
		{ 0x00010000, 0x0e000},
		{ 0xffffffff, 0x00000}
	}, mem_tbl_5755[] = {
		{ 0x00000200, 0x00008},
		{ 0x00004000, 0x00800},
		{ 0x00006000, 0x00800},
		{ 0x00008000, 0x02000},
		{ 0x00010000, 0x0c000},
		{ 0xffffffff, 0x00000}
	}, mem_tbl_5906[] = {
		{ 0x00000200, 0x00008},
		{ 0x00004000, 0x00400},
		{ 0x00006000, 0x00400},
		{ 0x00008000, 0x01000},
		{ 0x00010000, 0x01000},
		{ 0xffffffff, 0x00000}
	}, mem_tbl_5717[] = {
		{ 0x00000200, 0x00008},
		{ 0x00010000, 0x0a000},
		{ 0x00020000, 0x13c00},
		{ 0xffffffff, 0x00000}
	}, mem_tbl_57765[] = {
		{ 0x00000200, 0x00008},
		{ 0x00004000, 0x00800},
		{ 0x00006000, 0x09800},
		{ 0x00010000, 0x0a000},
		{ 0xffffffff, 0x00000}
	};
	struct mem_entry *mem_tbl;
	int err = 0;
	int i;

	if (tg3_flag(tp, 5717_PLUS))
		mem_tbl = mem_tbl_5717;
	else if (tg3_flag(tp, 57765_CLASS) ||
		 tg3_asic_rev(tp) == ASIC_REV_5762)
		mem_tbl = mem_tbl_57765;
	else if (tg3_flag(tp, 5755_PLUS))
		mem_tbl = mem_tbl_5755;
	else if (tg3_asic_rev(tp) == ASIC_REV_5906)
		mem_tbl = mem_tbl_5906;
	else if (tg3_flag(tp, 5705_PLUS))
		mem_tbl = mem_tbl_5705;
	else
		mem_tbl = mem_tbl_570x;

	for (i = 0; mem_tbl[i].offset != 0xffffffff; i++) {
		err = tg3_do_mem_test(tp, mem_tbl[i].offset, mem_tbl[i].len);
		if (err)
			break;
	}

	return err;
}
