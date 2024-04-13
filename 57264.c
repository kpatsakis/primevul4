static int snd_msnd_send_dsp_cmd_chk(struct snd_msnd *chip, u8 cmd)
{
	if (snd_msnd_send_dsp_cmd(chip, cmd) == 0)
		return 0;
	snd_msnd_dsp_full_reset(chip->card);
	return snd_msnd_send_dsp_cmd(chip, cmd);
}
