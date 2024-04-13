irqreturn_t floppy_interrupt(int irq, void *dev_id)
{
	int do_print;
	unsigned long f;
	void (*handler)(void) = do_floppy;

	lasthandler = handler;
	interruptjiffies = jiffies;

	f = claim_dma_lock();
	fd_disable_dma();
	release_dma_lock(f);

	do_floppy = NULL;
	if (fdc >= N_FDC || FDCS->address == -1) {
		/* we don't even know which FDC is the culprit */
		pr_info("DOR0=%x\n", fdc_state[0].dor);
		pr_info("floppy interrupt on bizarre fdc %d\n", fdc);
		pr_info("handler=%pf\n", handler);
		is_alive(__func__, "bizarre fdc");
		return IRQ_NONE;
	}

	FDCS->reset = 0;
	/* We have to clear the reset flag here, because apparently on boxes
	 * with level triggered interrupts (PS/2, Sparc, ...), it is needed to
	 * emit SENSEI's to clear the interrupt line. And FDCS->reset blocks the
	 * emission of the SENSEI's.
	 * It is OK to emit floppy commands because we are in an interrupt
	 * handler here, and thus we have to fear no interference of other
	 * activity.
	 */

	do_print = !handler && print_unex && initialized;

	inr = result();
	if (do_print)
		print_result("unexpected interrupt", inr);
	if (inr == 0) {
		int max_sensei = 4;
		do {
			output_byte(FD_SENSEI);
			inr = result();
			if (do_print)
				print_result("sensei", inr);
			max_sensei--;
		} while ((ST0 & 0x83) != UNIT(current_drive) &&
			 inr == 2 && max_sensei);
	}
	if (!handler) {
		FDCS->reset = 1;
		return IRQ_NONE;
	}
	schedule_bh(handler);
	is_alive(__func__, "normal interrupt end");

	/* FIXME! Was it really for us? */
	return IRQ_HANDLED;
}
