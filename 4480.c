void setExperimentalSettings(QuicServerConnectionState& conn) {
  conn.lossState.reorderingThreshold =
      std::numeric_limits<decltype(conn.lossState.reorderingThreshold)>::max();
}