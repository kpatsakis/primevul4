static int airspy_set_if_gain(struct airspy *s)
{
	int ret;
	u8 u8tmp;

	dev_dbg(s->dev, "val=%d->%d\n", s->if_gain->cur.val, s->if_gain->val);

	ret = airspy_ctrl_msg(s, CMD_SET_VGA_GAIN, 0, s->if_gain->val,
			&u8tmp, 1);
	if (ret)
		dev_dbg(s->dev, "failed=%d\n", ret);

	return ret;
}
