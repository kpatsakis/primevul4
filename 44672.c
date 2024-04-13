static void check_wp(void)
{
	if (test_bit(FD_VERIFY_BIT, &DRS->flags)) {
					/* check write protection */
		output_byte(FD_GETSTATUS);
		output_byte(UNIT(current_drive));
		if (result() != 1) {
			FDCS->reset = 1;
			return;
		}
		clear_bit(FD_VERIFY_BIT, &DRS->flags);
		clear_bit(FD_NEED_TWADDLE_BIT, &DRS->flags);
		debug_dcl(DP->flags,
			  "checking whether disk is write protected\n");
		debug_dcl(DP->flags, "wp=%x\n", ST3 & 0x40);
		if (!(ST3 & 0x40))
			set_bit(FD_DISK_WRITABLE_BIT, &DRS->flags);
		else
			clear_bit(FD_DISK_WRITABLE_BIT, &DRS->flags);
	}
}
