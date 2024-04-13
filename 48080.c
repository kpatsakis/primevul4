static void string_addr_inc(struct x86_emulate_ctxt *ctxt, int reg,
		struct operand *op)
{
	int df = (ctxt->eflags & EFLG_DF) ? -op->count : op->count;

	register_address_increment(ctxt, reg, df * op->bytes);
	op->addr.mem.ea = register_address(ctxt, reg);
}
