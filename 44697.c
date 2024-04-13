static bool floppy_available(int drive)
{
	if (!(allowed_drive_mask & (1 << drive)))
		return false;
	if (fdc_state[FDC(drive)].version == FDC_NONE)
		return false;
	return true;
}
