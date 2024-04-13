void updateTransportParamsFromTicket(
    QuicServerConnectionState& conn,
    uint64_t idleTimeout,
    uint64_t maxRecvPacketSize,
    uint64_t initialMaxData,
    uint64_t initialMaxStreamDataBidiLocal,
    uint64_t initialMaxStreamDataBidiRemote,
    uint64_t initialMaxStreamDataUni,
    uint64_t initialMaxStreamsBidi,
    uint64_t initialMaxStreamsUni) {
  conn.transportSettings.idleTimeout = std::chrono::milliseconds(idleTimeout);
  conn.transportSettings.maxRecvPacketSize = maxRecvPacketSize;

  conn.transportSettings.advertisedInitialConnectionWindowSize = initialMaxData;
  conn.transportSettings.advertisedInitialBidiLocalStreamWindowSize =
      initialMaxStreamDataBidiLocal;
  conn.transportSettings.advertisedInitialBidiRemoteStreamWindowSize =
      initialMaxStreamDataBidiRemote;
  conn.transportSettings.advertisedInitialUniStreamWindowSize =
      initialMaxStreamDataUni;
  updateFlowControlStateWithSettings(
      conn.flowControlState, conn.transportSettings);

  conn.transportSettings.advertisedInitialMaxStreamsBidi =
      initialMaxStreamsBidi;
  conn.transportSettings.advertisedInitialMaxStreamsUni = initialMaxStreamsUni;
}