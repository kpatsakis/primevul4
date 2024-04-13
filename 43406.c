struct snd_kcontrol *snd_ctl_new1(const struct snd_kcontrol_new *ncontrol,
				  void *private_data)
{
	struct snd_kcontrol kctl;
	unsigned int access;
	
	if (snd_BUG_ON(!ncontrol || !ncontrol->info))
		return NULL;
	memset(&kctl, 0, sizeof(kctl));
	kctl.id.iface = ncontrol->iface;
	kctl.id.device = ncontrol->device;
	kctl.id.subdevice = ncontrol->subdevice;
	if (ncontrol->name) {
		strlcpy(kctl.id.name, ncontrol->name, sizeof(kctl.id.name));
		if (strcmp(ncontrol->name, kctl.id.name) != 0)
			pr_warn("ALSA: Control name '%s' truncated to '%s'\n",
				ncontrol->name, kctl.id.name);
	}
	kctl.id.index = ncontrol->index;
	kctl.count = ncontrol->count ? ncontrol->count : 1;
	access = ncontrol->access == 0 ? SNDRV_CTL_ELEM_ACCESS_READWRITE :
		 (ncontrol->access & (SNDRV_CTL_ELEM_ACCESS_READWRITE|
				      SNDRV_CTL_ELEM_ACCESS_VOLATILE|
				      SNDRV_CTL_ELEM_ACCESS_INACTIVE|
				      SNDRV_CTL_ELEM_ACCESS_TLV_READWRITE|
				      SNDRV_CTL_ELEM_ACCESS_TLV_COMMAND|
				      SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK));
	kctl.info = ncontrol->info;
	kctl.get = ncontrol->get;
	kctl.put = ncontrol->put;
	kctl.tlv.p = ncontrol->tlv.p;
	kctl.private_value = ncontrol->private_value;
	kctl.private_data = private_data;
	return snd_ctl_new(&kctl, access);
}
