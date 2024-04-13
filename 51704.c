proto_reg_handoff_ppi(void)
{
    data_handle = find_dissector("data");
    ieee80211_radio_handle = find_dissector("wlan_radio");
    ppi_gps_handle = find_dissector("ppi_gps");
    ppi_vector_handle = find_dissector("ppi_vector");
    ppi_sensor_handle = find_dissector("ppi_sensor");
    ppi_antenna_handle = find_dissector("ppi_antenna");
    ppi_fnet_handle = find_dissector("ppi_fnet");

    dissector_add_uint("wtap_encap", WTAP_ENCAP_PPI, ppi_handle);
}
