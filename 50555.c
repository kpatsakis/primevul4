s32 brcmf_cfg80211_up(struct net_device *ndev)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	s32 err = 0;

	mutex_lock(&cfg->usr_sync);
	err = __brcmf_cfg80211_up(ifp);
	mutex_unlock(&cfg->usr_sync);

	return err;
}
