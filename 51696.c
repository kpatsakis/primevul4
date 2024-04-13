proto_register_pktap(void)
{
	static hf_register_info hf[] = {
	  { &hf_pktap_hdrlen,
	    { "Header length", "pktap.hdrlen",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_rectype,
	    { "Record type", "pktap.rectype",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_dlt,
	    { "DLT", "pktap.dlt",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_ifname,	/* fixed length *and* null-terminated */
	    { "Interface name", "pktap.ifname",
	      FT_STRINGZ, BASE_NONE, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_flags,
	    { "Flags", "pktap.flags",
	      FT_UINT32, BASE_HEX, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_pfamily,
	    { "Protocol family", "pktap.pfamily",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_llhdrlen,
	    { "Link-layer header length", "pktap.llhdrlen",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_lltrlrlen,
	    { "Link-layer trailer length", "pktap.lltrlrlen",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_pid,
	    { "Process ID", "pktap.pid",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_cmdname,	/* fixed length *and* null-terminated */
	    { "Command name", "pktap.cmdname",
	      FT_STRINGZ, BASE_NONE, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_svc_class,
	    { "Service class", "pktap.svc_class",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_iftype,
	    { "Interface type", "pktap.iftype",
	      FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_ifunit,
	    { "Interface unit", "pktap.ifunit",
	      FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_epid,
	    { "Effective process ID", "pktap.epid",
	      FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL } },
	  { &hf_pktap_ecmdname,	/* fixed length *and* null-terminated */
	    { "Effective command name", "pktap.ecmdname",
	      FT_STRINGZ, BASE_NONE, NULL, 0x0, NULL, HFILL } },
	};

	static gint *ett[] = {
		&ett_pktap,
	};

	static ei_register_info ei[] = {
	    { &ei_pktap_hdrlen_too_short,
	      { "pktap.hdrlen_too_short", PI_MALFORMED, PI_ERROR,
	        "Header length is too short", EXPFILL }},
	};

	expert_module_t* expert_pktap;

	proto_pktap = proto_register_protocol("PKTAP packet header", "PKTAP",
	    "pktap");
	proto_register_field_array(proto_pktap, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));
	expert_pktap = expert_register_protocol(proto_pktap);
	expert_register_field_array(expert_pktap, ei, array_length(ei));

	pktap_handle = register_dissector("pktap", dissect_pktap, proto_pktap);
}
