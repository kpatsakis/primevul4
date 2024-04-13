static int result(void)
{
	int i;
	int status = 0;

	for (i = 0; i < MAX_REPLIES; i++) {
		status = wait_til_ready();
		if (status < 0)
			break;
		status &= STATUS_DIR | STATUS_READY | STATUS_BUSY | STATUS_DMA;
		if ((status & ~STATUS_BUSY) == STATUS_READY) {
			resultjiffies = jiffies;
			resultsize = i;
			return i;
		}
		if (status == (STATUS_DIR | STATUS_READY | STATUS_BUSY))
			reply_buffer[i] = fd_inb(FD_DATA);
		else
			break;
	}
	if (initialized) {
		DPRINT("get result error. Fdc=%d Last status=%x Read bytes=%d\n",
		       fdc, status, i);
		show_floppy();
	}
	FDCS->reset = 1;
	return -1;
}
