fst_op_lower(struct fst_port_info *port, unsigned int outputs)
{
	outputs = ~outputs & FST_RDL(port->card, v24OpSts[port->index]);
	FST_WRL(port->card, v24OpSts[port->index], outputs);

	if (port->run)
		fst_issue_cmd(port, SETV24O);
}
