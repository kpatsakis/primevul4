static int roland_load_info(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_info *info)
{
	static const char *const names[] = { "High Load", "Light Load" };

	return snd_ctl_enum_info(info, 1, 2, names);
}
