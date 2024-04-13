static int airspy_set_lna_gain(struct airspy *s)
{
	int ret;
	u8 u8tmp;

	dev_dbg(s->dev, "lna auto=%d->%d val=%d->%d\n",
			s->lna_gain_auto->cur.val, s->lna_gain_auto->val,
			s->lna_gain->cur.val, s->lna_gain->val);

	ret = airspy_ctrl_msg(s, CMD_SET_LNA_AGC, 0, s->lna_gain_auto->val,
			&u8tmp, 1);
	if (ret)
		goto err;

	if (s->lna_gain_auto->val == false) {
		ret = airspy_ctrl_msg(s, CMD_SET_LNA_GAIN, 0, s->lna_gain->val,
				&u8tmp, 1);
		if (ret)
			goto err;
	}
err:
	if (ret)
		dev_dbg(s->dev, "failed=%d\n", ret);

	return ret;
}
