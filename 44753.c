static void reset_interrupt(void)
{
	debugt(__func__, "");
	result();		/* get the status ready for set_fdc */
	if (FDCS->reset) {
		pr_info("reset set in interrupt, calling %pf\n", cont->error);
		cont->error();	/* a reset just after a reset. BAD! */
	}
	cont->redo();
}
