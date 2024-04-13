static void cm_format_path_from_lap(struct cm_id_private *cm_id_priv,
				    struct ib_sa_path_rec *path,
				    struct cm_lap_msg *lap_msg)
{
	memset(path, 0, sizeof *path);
	path->dgid = lap_msg->alt_local_gid;
	path->sgid = lap_msg->alt_remote_gid;
	path->dlid = lap_msg->alt_local_lid;
	path->slid = lap_msg->alt_remote_lid;
	path->flow_label = cm_lap_get_flow_label(lap_msg);
	path->hop_limit = lap_msg->alt_hop_limit;
	path->traffic_class = cm_lap_get_traffic_class(lap_msg);
	path->reversible = 1;
	path->pkey = cm_id_priv->pkey;
	path->sl = cm_lap_get_sl(lap_msg);
	path->mtu_selector = IB_SA_EQ;
	path->mtu = cm_id_priv->path_mtu;
	path->rate_selector = IB_SA_EQ;
	path->rate = cm_lap_get_packet_rate(lap_msg);
	path->packet_life_time_selector = IB_SA_EQ;
	path->packet_life_time = cm_lap_get_local_ack_timeout(lap_msg);
	path->packet_life_time -= (path->packet_life_time > 0);
}
