netdev_tx_t ieee80211_monitor_start_xmit(struct sk_buff *skb,
					 struct net_device *dev)
{
	struct ieee80211_local *local = wdev_priv(dev->ieee80211_ptr);
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct ieee80211_channel *chan;
	struct ieee80211_radiotap_header *prthdr =
		(struct ieee80211_radiotap_header *)skb->data;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hdr *hdr;
	struct ieee80211_sub_if_data *tmp_sdata, *sdata;
	u16 len_rthdr;
	int hdrlen;

	/* check for not even having the fixed radiotap header part */
	if (unlikely(skb->len < sizeof(struct ieee80211_radiotap_header)))
		goto fail; /* too short to be possibly valid */

	/* is it a header version we can trust to find length from? */
	if (unlikely(prthdr->it_version))
		goto fail; /* only version 0 is supported */

	/* then there must be a radiotap header with a length we can use */
	len_rthdr = ieee80211_get_radiotap_len(skb->data);

	/* does the skb contain enough to deliver on the alleged length? */
	if (unlikely(skb->len < len_rthdr))
		goto fail; /* skb too short for claimed rt header extent */

	/*
	 * fix up the pointers accounting for the radiotap
	 * header still being in there.  We are being given
	 * a precooked IEEE80211 header so no need for
	 * normal processing
	 */
	skb_set_mac_header(skb, len_rthdr);
	/*
	 * these are just fixed to the end of the rt area since we
	 * don't have any better information and at this point, nobody cares
	 */
	skb_set_network_header(skb, len_rthdr);
	skb_set_transport_header(skb, len_rthdr);

	if (skb->len < len_rthdr + 2)
		goto fail;

	hdr = (struct ieee80211_hdr *)(skb->data + len_rthdr);
	hdrlen = ieee80211_hdrlen(hdr->frame_control);

	if (skb->len < len_rthdr + hdrlen)
		goto fail;

	/*
	 * Initialize skb->protocol if the injected frame is a data frame
	 * carrying a rfc1042 header
	 */
	if (ieee80211_is_data(hdr->frame_control) &&
	    skb->len >= len_rthdr + hdrlen + sizeof(rfc1042_header) + 2) {
		u8 *payload = (u8 *)hdr + hdrlen;

		if (ether_addr_equal(payload, rfc1042_header))
			skb->protocol = cpu_to_be16((payload[6] << 8) |
						    payload[7]);
	}

	memset(info, 0, sizeof(*info));

	info->flags = IEEE80211_TX_CTL_REQ_TX_STATUS |
		      IEEE80211_TX_CTL_INJECTED;

	/* process and remove the injection radiotap header */
	if (!ieee80211_parse_tx_radiotap(skb))
		goto fail;

	rcu_read_lock();

	/*
	 * We process outgoing injected frames that have a local address
	 * we handle as though they are non-injected frames.
	 * This code here isn't entirely correct, the local MAC address
	 * isn't always enough to find the interface to use; for proper
	 * VLAN/WDS support we will need a different mechanism (which
	 * likely isn't going to be monitor interfaces).
	 */
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	list_for_each_entry_rcu(tmp_sdata, &local->interfaces, list) {
		if (!ieee80211_sdata_running(tmp_sdata))
			continue;
		if (tmp_sdata->vif.type == NL80211_IFTYPE_MONITOR ||
		    tmp_sdata->vif.type == NL80211_IFTYPE_AP_VLAN ||
		    tmp_sdata->vif.type == NL80211_IFTYPE_WDS)
			continue;
		if (ether_addr_equal(tmp_sdata->vif.addr, hdr->addr2)) {
			sdata = tmp_sdata;
			break;
		}
	}

	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
	if (!chanctx_conf) {
		tmp_sdata = rcu_dereference(local->monitor_sdata);
		if (tmp_sdata)
			chanctx_conf =
				rcu_dereference(tmp_sdata->vif.chanctx_conf);
	}

	if (chanctx_conf)
		chan = chanctx_conf->def.chan;
	else if (!local->use_chanctx)
		chan = local->_oper_chandef.chan;
	else
		goto fail_rcu;

	/*
	 * Frame injection is not allowed if beaconing is not allowed
	 * or if we need radar detection. Beaconing is usually not allowed when
	 * the mode or operation (Adhoc, AP, Mesh) does not support DFS.
	 * Passive scan is also used in world regulatory domains where
	 * your country is not known and as such it should be treated as
	 * NO TX unless the channel is explicitly allowed in which case
	 * your current regulatory domain would not have the passive scan
	 * flag.
	 *
	 * Since AP mode uses monitor interfaces to inject/TX management
	 * frames we can make AP mode the exception to this rule once it
	 * supports radar detection as its implementation can deal with
	 * radar detection by itself. We can do that later by adding a
	 * monitor flag interfaces used for AP support.
	 */
	if ((chan->flags & (IEEE80211_CHAN_NO_IR | IEEE80211_CHAN_RADAR)))
		goto fail_rcu;

	ieee80211_xmit(sdata, skb, chan->band);
	rcu_read_unlock();

	return NETDEV_TX_OK;

fail_rcu:
	rcu_read_unlock();
fail:
	dev_kfree_skb(skb);
	return NETDEV_TX_OK; /* meaning, we dealt with the skb */
}
