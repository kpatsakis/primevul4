void resetCongestionAndRttState(QuicServerConnectionState& conn) {
  CHECK(conn.congestionControllerFactory)
      << "CongestionControllerFactory is not set.";
  conn.congestionController =
      conn.congestionControllerFactory->makeCongestionController(
          conn, conn.transportSettings.defaultCongestionController);
  conn.lossState.srtt = 0us;
  conn.lossState.lrtt = 0us;
  conn.lossState.rttvar = 0us;
  conn.lossState.mrtt = kDefaultMinRtt;
}