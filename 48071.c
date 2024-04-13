static int em_push_sreg(struct x86_emulate_ctxt *ctxt)
{
	int seg = ctxt->src2.val;

	ctxt->src.val = get_segment_selector(ctxt, seg);
	if (ctxt->op_bytes == 4) {
		rsp_increment(ctxt, -2);
		ctxt->op_bytes = 2;
	}

	return em_push(ctxt);
}
