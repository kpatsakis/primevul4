static int snd_msnd_calibrate_adc(struct snd_msnd *chip, u16 srate)
{
	snd_printdd("snd_msnd_calibrate_adc(%i)\n", srate);
	writew(srate, chip->SMA + SMA_wCalFreqAtoD);
	if (chip->calibrate_signal == 0)
		writew(readw(chip->SMA + SMA_wCurrHostStatusFlags)
		       | 0x0001, chip->SMA + SMA_wCurrHostStatusFlags);
	else
		writew(readw(chip->SMA + SMA_wCurrHostStatusFlags)
		       & ~0x0001, chip->SMA + SMA_wCurrHostStatusFlags);
	if (snd_msnd_send_word(chip, 0, 0, HDEXAR_CAL_A_TO_D) == 0 &&
	    snd_msnd_send_dsp_cmd_chk(chip, HDEX_AUX_REQ) == 0) {
		schedule_timeout_interruptible(msecs_to_jiffies(333));
		return 0;
	}
	printk(KERN_WARNING LOGNAME ": ADC calibration failed\n");
	return -EIO;
}
