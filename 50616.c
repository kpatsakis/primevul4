static s32 brcmf_set_retry(struct net_device *ndev, u32 retry, bool l)
{
	s32 err = 0;
	u32 cmd = (l ? BRCMF_C_SET_LRL : BRCMF_C_SET_SRL);

	err = brcmf_fil_cmd_int_set(netdev_priv(ndev), cmd, retry);
	if (err) {
		brcmf_err("cmd (%d) , error (%d)\n", cmd, err);
		return err;
	}
	return err;
}
