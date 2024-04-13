static void snd_ctl_elem_user_free(struct snd_kcontrol *kcontrol)
{
	struct user_element *ue = kcontrol->private_data;

	kfree(ue->tlv_data);
	kfree(ue->priv_data);
	kfree(ue);
}
