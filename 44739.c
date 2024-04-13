static int output_byte(char byte)
{
	int status = wait_til_ready();

	if (status < 0)
		return -1;

	if (is_ready_state(status)) {
		fd_outb(byte, FD_DATA);
		output_log[output_log_pos].data = byte;
		output_log[output_log_pos].status = status;
		output_log[output_log_pos].jiffies = jiffies;
		output_log_pos = (output_log_pos + 1) % OLOGSIZE;
		return 0;
	}
	FDCS->reset = 1;
	if (initialized) {
		DPRINT("Unable to send byte %x to FDC. Fdc=%x Status=%x\n",
		       byte, fdc, status);
		show_floppy();
	}
	return -1;
}
