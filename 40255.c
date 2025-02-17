void iscsi_set_connection_parameters(
	struct iscsi_conn_ops *ops,
	struct iscsi_param_list *param_list)
{
	char *tmpptr;
	struct iscsi_param *param;

	pr_debug("---------------------------------------------------"
			"---------------\n");
	list_for_each_entry(param, &param_list->param_list, p_list) {
		/*
		 * Special case to set MAXXMITDATASEGMENTLENGTH from the
		 * target requested MaxRecvDataSegmentLength, even though
		 * this key is not sent over the wire.
		 */
		if (!strcmp(param->name, MAXXMITDATASEGMENTLENGTH)) {
			if (param_list->iser == true)
				continue;

			ops->MaxXmitDataSegmentLength =
				simple_strtoul(param->value, &tmpptr, 0);
			pr_debug("MaxXmitDataSegmentLength:     %s\n",
				param->value);
		}

		if (!IS_PSTATE_ACCEPTOR(param) && !IS_PSTATE_PROPOSER(param))
			continue;
		if (!strcmp(param->name, AUTHMETHOD)) {
			pr_debug("AuthMethod:                   %s\n",
				param->value);
		} else if (!strcmp(param->name, HEADERDIGEST)) {
			ops->HeaderDigest = !strcmp(param->value, CRC32C);
			pr_debug("HeaderDigest:                 %s\n",
				param->value);
		} else if (!strcmp(param->name, DATADIGEST)) {
			ops->DataDigest = !strcmp(param->value, CRC32C);
			pr_debug("DataDigest:                   %s\n",
				param->value);
		} else if (!strcmp(param->name, MAXRECVDATASEGMENTLENGTH)) {
			/*
			 * At this point iscsi_check_acceptor_state() will have
			 * set ops->MaxRecvDataSegmentLength from the original
			 * initiator provided value.
			 */
			pr_debug("MaxRecvDataSegmentLength:     %u\n",
				ops->MaxRecvDataSegmentLength);
		} else if (!strcmp(param->name, OFMARKER)) {
			ops->OFMarker = !strcmp(param->value, YES);
			pr_debug("OFMarker:                     %s\n",
				param->value);
		} else if (!strcmp(param->name, IFMARKER)) {
			ops->IFMarker = !strcmp(param->value, YES);
			pr_debug("IFMarker:                     %s\n",
				param->value);
		} else if (!strcmp(param->name, OFMARKINT)) {
			ops->OFMarkInt =
				simple_strtoul(param->value, &tmpptr, 0);
			pr_debug("OFMarkInt:                    %s\n",
				param->value);
		} else if (!strcmp(param->name, IFMARKINT)) {
			ops->IFMarkInt =
				simple_strtoul(param->value, &tmpptr, 0);
			pr_debug("IFMarkInt:                    %s\n",
				param->value);
		} else if (!strcmp(param->name, INITIATORRECVDATASEGMENTLENGTH)) {
			ops->InitiatorRecvDataSegmentLength =
				simple_strtoul(param->value, &tmpptr, 0);
			pr_debug("InitiatorRecvDataSegmentLength: %s\n",
				param->value);
			ops->MaxRecvDataSegmentLength =
					ops->InitiatorRecvDataSegmentLength;
			pr_debug("Set MRDSL from InitiatorRecvDataSegmentLength\n");
		} else if (!strcmp(param->name, TARGETRECVDATASEGMENTLENGTH)) {
			ops->TargetRecvDataSegmentLength =
				simple_strtoul(param->value, &tmpptr, 0);
			pr_debug("TargetRecvDataSegmentLength:  %s\n",
				param->value);
			ops->MaxXmitDataSegmentLength =
					ops->TargetRecvDataSegmentLength;
			pr_debug("Set MXDSL from TargetRecvDataSegmentLength\n");
		}
	}
	pr_debug("----------------------------------------------------"
			"--------------\n");
}
