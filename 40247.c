void iscsi_dump_sess_ops(struct iscsi_sess_ops *sess_ops)
{
	pr_debug("InitiatorName: %s\n", sess_ops->InitiatorName);
	pr_debug("InitiatorAlias: %s\n", sess_ops->InitiatorAlias);
	pr_debug("TargetName: %s\n", sess_ops->TargetName);
	pr_debug("TargetAlias: %s\n", sess_ops->TargetAlias);
	pr_debug("TargetPortalGroupTag: %hu\n",
			sess_ops->TargetPortalGroupTag);
	pr_debug("MaxConnections: %hu\n", sess_ops->MaxConnections);
	pr_debug("InitialR2T: %s\n",
			(sess_ops->InitialR2T) ? "Yes" : "No");
	pr_debug("ImmediateData: %s\n", (sess_ops->ImmediateData) ?
			"Yes" : "No");
	pr_debug("MaxBurstLength: %u\n", sess_ops->MaxBurstLength);
	pr_debug("FirstBurstLength: %u\n", sess_ops->FirstBurstLength);
	pr_debug("DefaultTime2Wait: %hu\n", sess_ops->DefaultTime2Wait);
	pr_debug("DefaultTime2Retain: %hu\n",
			sess_ops->DefaultTime2Retain);
	pr_debug("MaxOutstandingR2T: %hu\n",
			sess_ops->MaxOutstandingR2T);
	pr_debug("DataPDUInOrder: %s\n",
			(sess_ops->DataPDUInOrder) ? "Yes" : "No");
	pr_debug("DataSequenceInOrder: %s\n",
			(sess_ops->DataSequenceInOrder) ? "Yes" : "No");
	pr_debug("ErrorRecoveryLevel: %hu\n",
			sess_ops->ErrorRecoveryLevel);
	pr_debug("SessionType: %s\n", (sess_ops->SessionType) ?
			"Discovery" : "Normal");
}
