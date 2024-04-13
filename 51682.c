static int airspy_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct airspy *s = container_of(ctrl->handler, struct airspy, hdl);
	int ret;

	switch (ctrl->id) {
	case  V4L2_CID_RF_TUNER_LNA_GAIN_AUTO:
	case  V4L2_CID_RF_TUNER_LNA_GAIN:
		ret = airspy_set_lna_gain(s);
		break;
	case  V4L2_CID_RF_TUNER_MIXER_GAIN_AUTO:
	case  V4L2_CID_RF_TUNER_MIXER_GAIN:
		ret = airspy_set_mixer_gain(s);
		break;
	case  V4L2_CID_RF_TUNER_IF_GAIN:
		ret = airspy_set_if_gain(s);
		break;
	default:
		dev_dbg(s->dev, "unknown ctrl: id=%d name=%s\n",
				ctrl->id, ctrl->name);
		ret = -EINVAL;
	}

	return ret;
}
