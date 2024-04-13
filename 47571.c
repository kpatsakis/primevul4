microcode_pointer(struct microcode_intel **mc_saved,
		  unsigned long *mc_saved_in_initrd,
		  unsigned long initrd_start, int mc_saved_count)
{
	int i;

	for (i = 0; i < mc_saved_count; i++)
		mc_saved[i] = (struct microcode_intel *)
			      (mc_saved_in_initrd[i] + initrd_start);
}
