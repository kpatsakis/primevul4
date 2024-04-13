static int roland_load_get(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_value *value)
{
	value->value.enumerated.item[0] = kcontrol->private_value;
	return 0;
}
