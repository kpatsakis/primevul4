static int snd_msnd_write_cfg_irq(int cfg, int num, u16 irq)
{
	if (snd_msnd_write_cfg(cfg, IREG_LOGDEVICE, num))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_IRQ_NUMBER, LOBYTE(irq)))
		return -EIO;
	if (snd_msnd_write_cfg(cfg, IREG_IRQ_TYPE, IRQTYPE_EDGE))
		return -EIO;
	return 0;
}
