static int snd_msnd_write_cfg_io1(int cfg, int num, u16 io)
{
	if (snd_msnd_write_cfg(cfg, IREG_LOGDEVICE, num))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_IO1_BASEHI, HIBYTE(io)))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_IO1_BASELO, LOBYTE(io)))
		return -EIO;
	return 0;
}
