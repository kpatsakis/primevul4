static int ati_remote2_get_mode_mask(char *buffer,
				     const struct kernel_param *kp)
{
	pr_debug("%s()\n", __func__);

	return sprintf(buffer, "0x%02x", *(unsigned int *)kp->arg);
}
