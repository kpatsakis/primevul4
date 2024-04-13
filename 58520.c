brcmf_alloc_internal_escan_request(struct wiphy *wiphy, u32 n_netinfo) {
	struct cfg80211_scan_request *req;
	size_t req_size;

	req_size = sizeof(*req) +
		   n_netinfo * sizeof(req->channels[0]) +
		   n_netinfo * sizeof(*req->ssids);

	req = kzalloc(req_size, GFP_KERNEL);
	if (req) {
		req->wiphy = wiphy;
		req->ssids = (void *)(&req->channels[0]) +
			     n_netinfo * sizeof(req->channels[0]);
	}
	return req;
}
