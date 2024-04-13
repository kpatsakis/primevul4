static int roland_load_put(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_value *value)
{
	struct snd_usb_midi *umidi = kcontrol->private_data;
	int changed;

	if (value->value.enumerated.item[0] > 1)
		return -EINVAL;
	mutex_lock(&umidi->mutex);
	changed = value->value.enumerated.item[0] != kcontrol->private_value;
	if (changed)
		kcontrol->private_value = value->value.enumerated.item[0];
	mutex_unlock(&umidi->mutex);
	return changed;
}
