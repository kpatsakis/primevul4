static int floppy_resume(struct device *dev)
{
	int fdc;

	for (fdc = 0; fdc < N_FDC; fdc++)
		if (FDCS->address != -1)
			user_reset_fdc(-1, FD_RESET_ALWAYS, false);

	return 0;
}
