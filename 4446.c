void onConnectionMigration(
    QuicServerConnectionState& conn,
    const folly::SocketAddress& newPeerAddress,
    bool isIntentional) {
  if (conn.migrationState.numMigrations >= kMaxNumMigrationsAllowed) {
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(
          0,
          QuicTransportStatsCallback::toString(
              PacketDropReason::PEER_ADDRESS_CHANGE));
    }
    QUIC_STATS(
        conn.statsCallback,
        onPacketDropped,
        PacketDropReason::PEER_ADDRESS_CHANGE);
    throw QuicTransportException(
        "Too many migrations", TransportErrorCode::INVALID_MIGRATION);
  }
  ++conn.migrationState.numMigrations;

  bool hasPendingPathChallenge = conn.pendingEvents.pathChallenge.has_value();
  // Clear any pending path challenge frame that is not sent
  conn.pendingEvents.pathChallenge = folly::none;

  auto& previousPeerAddresses = conn.migrationState.previousPeerAddresses;
  auto it = std::find(
      previousPeerAddresses.begin(),
      previousPeerAddresses.end(),
      newPeerAddress);
  if (it == previousPeerAddresses.end()) {
    // Send new path challenge
    uint64_t pathData;
    folly::Random::secureRandom(&pathData, sizeof(pathData));
    conn.pendingEvents.pathChallenge = PathChallengeFrame(pathData);

    // If we are already in the middle of a migration reset
    // the available bytes in the rate-limited window, but keep the
    // window.
    conn.pathValidationLimiter =
        std::make_unique<PendingPathRateLimiter>(conn.udpSendPacketLen);
  } else {
    previousPeerAddresses.erase(it);
  }

  // At this point, path validation scheduled, writable bytes limit set
  // However if this is NAT rebinding, keep congestion state unchanged
  bool isNATRebinding = maybeNATRebinding(newPeerAddress, conn.peerAddress);

  // Cancel current path validation if any
  if (hasPendingPathChallenge || conn.outstandingPathValidation) {
    conn.pendingEvents.schedulePathValidationTimeout = false;
    conn.outstandingPathValidation = folly::none;

    // Only change congestion & rtt state if not NAT rebinding
    if (!isNATRebinding) {
      recoverOrResetCongestionAndRttState(conn, newPeerAddress);
    }
  } else {
    // Only add validated addresses to previousPeerAddresses
    conn.migrationState.previousPeerAddresses.push_back(conn.peerAddress);

    // Only change congestion & rtt state if not NAT rebinding
    if (!isNATRebinding) {
      // Current peer address is validated,
      // remember its congestion state and rtt stats
      CongestionAndRttState state = moveCurrentCongestionAndRttState(conn);
      recoverOrResetCongestionAndRttState(conn, newPeerAddress);
      conn.migrationState.lastCongestionAndRtt = std::move(state);
    }
  }

  if (conn.qLogger) {
    conn.qLogger->addConnectionMigrationUpdate(isIntentional);
  }
  conn.peerAddress = newPeerAddress;
}