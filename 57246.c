static int snd_msnd_activate_logical(int cfg, int num)
{
	if (snd_msnd_write_cfg(cfg, IREG_LOGDEVICE, num))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_ACTIVATE, LD_ACTIVATE))
		return -EIO;
	return 0;
}
