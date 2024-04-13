static inline int cm_is_active_peer(__be64 local_ca_guid, __be64 remote_ca_guid,
				    __be32 local_qpn, __be32 remote_qpn)
{
	return (be64_to_cpu(local_ca_guid) > be64_to_cpu(remote_ca_guid) ||
		((local_ca_guid == remote_ca_guid) &&
		 (be32_to_cpu(local_qpn) > be32_to_cpu(remote_qpn))));
}
