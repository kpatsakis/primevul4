static int airspy_set_mixer_gain(struct airspy *s)
{
	int ret;
	u8 u8tmp;

	dev_dbg(s->dev, "mixer auto=%d->%d val=%d->%d\n",
			s->mixer_gain_auto->cur.val, s->mixer_gain_auto->val,
			s->mixer_gain->cur.val, s->mixer_gain->val);

	ret = airspy_ctrl_msg(s, CMD_SET_MIXER_AGC, 0, s->mixer_gain_auto->val,
			&u8tmp, 1);
	if (ret)
		goto err;

	if (s->mixer_gain_auto->val == false) {
		ret = airspy_ctrl_msg(s, CMD_SET_MIXER_GAIN, 0,
				s->mixer_gain->val, &u8tmp, 1);
		if (ret)
			goto err;
	}
err:
	if (ret)
		dev_dbg(s->dev, "failed=%d\n", ret);

	return ret;
}
