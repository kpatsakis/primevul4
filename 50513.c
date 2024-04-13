void brcmf_cfg80211_arm_vif_event(struct brcmf_cfg80211_info *cfg,
				  struct brcmf_cfg80211_vif *vif)
{
	struct brcmf_cfg80211_vif_event *event = &cfg->vif_event;

	spin_lock(&event->vif_event_lock);
	event->vif = vif;
	event->action = 0;
	spin_unlock(&event->vif_event_lock);
}
