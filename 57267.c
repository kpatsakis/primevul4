static int snd_msnd_write_cfg_io0(int cfg, int num, u16 io)
{
	if (snd_msnd_write_cfg(cfg, IREG_LOGDEVICE, num))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_IO0_BASEHI, HIBYTE(io)))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_IO0_BASELO, LOBYTE(io)))
		return -EIO;
	return 0;
}
