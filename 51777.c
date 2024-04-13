void proto_reg_handoff_fp(void)
{
    dissector_handle_t fp_aal2_handle;

    rlc_bcch_handle           = find_dissector_add_dependency("rlc.bcch", proto_fp);
    mac_fdd_rach_handle       = find_dissector_add_dependency("mac.fdd.rach", proto_fp);
    mac_fdd_fach_handle       = find_dissector_add_dependency("mac.fdd.fach", proto_fp);
    mac_fdd_pch_handle        = find_dissector_add_dependency("mac.fdd.pch", proto_fp);
    mac_fdd_dch_handle        = find_dissector_add_dependency("mac.fdd.dch", proto_fp);
    mac_fdd_edch_handle       = find_dissector_add_dependency("mac.fdd.edch", proto_fp);
    mac_fdd_edch_type2_handle = find_dissector_add_dependency("mac.fdd.edch.type2", proto_fp);
    mac_fdd_hsdsch_handle     = find_dissector_add_dependency("mac.fdd.hsdsch", proto_fp);

    heur_dissector_add("udp", heur_dissect_fp, "FP over UDP", "fp_udp", proto_fp, HEURISTIC_DISABLE);

    fp_aal2_handle = create_dissector_handle(dissect_fp_aal2, proto_fp);
    dissector_add_uint("atm.aal2.type", TRAF_UMTS_FP, fp_aal2_handle);
}
