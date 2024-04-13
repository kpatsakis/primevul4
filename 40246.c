void iscsi_dump_conn_ops(struct iscsi_conn_ops *conn_ops)
{
	pr_debug("HeaderDigest: %s\n", (conn_ops->HeaderDigest) ?
				"CRC32C" : "None");
	pr_debug("DataDigest: %s\n", (conn_ops->DataDigest) ?
				"CRC32C" : "None");
	pr_debug("MaxRecvDataSegmentLength: %u\n",
				conn_ops->MaxRecvDataSegmentLength);
	pr_debug("OFMarker: %s\n", (conn_ops->OFMarker) ? "Yes" : "No");
	pr_debug("IFMarker: %s\n", (conn_ops->IFMarker) ? "Yes" : "No");
	if (conn_ops->OFMarker)
		pr_debug("OFMarkInt: %u\n", conn_ops->OFMarkInt);
	if (conn_ops->IFMarker)
		pr_debug("IFMarkInt: %u\n", conn_ops->IFMarkInt);
}
