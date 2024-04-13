void processClientInitialParams(
    QuicServerConnectionState& conn,
    const ClientTransportParameters& clientParams) {
  // TODO validate that we didn't receive original connection ID, stateless
  // reset token, or preferred address.
  auto maxData = getIntegerParameter(
      TransportParameterId::initial_max_data, clientParams.parameters);
  auto maxStreamDataBidiLocal = getIntegerParameter(
      TransportParameterId::initial_max_stream_data_bidi_local,
      clientParams.parameters);
  auto maxStreamDataBidiRemote = getIntegerParameter(
      TransportParameterId::initial_max_stream_data_bidi_remote,
      clientParams.parameters);
  auto maxStreamDataUni = getIntegerParameter(
      TransportParameterId::initial_max_stream_data_uni,
      clientParams.parameters);
  auto maxStreamsBidi = getIntegerParameter(
      TransportParameterId::initial_max_streams_bidi, clientParams.parameters);
  auto maxStreamsUni = getIntegerParameter(
      TransportParameterId::initial_max_streams_uni, clientParams.parameters);
  auto idleTimeout = getIntegerParameter(
      TransportParameterId::idle_timeout, clientParams.parameters);
  auto ackDelayExponent = getIntegerParameter(
      TransportParameterId::ack_delay_exponent, clientParams.parameters);
  auto packetSize = getIntegerParameter(
      TransportParameterId::max_packet_size, clientParams.parameters);
  auto partialReliability = getIntegerParameter(
      static_cast<TransportParameterId>(kPartialReliabilityParameterId),
      clientParams.parameters);
  auto activeConnectionIdLimit = getIntegerParameter(
      TransportParameterId::active_connection_id_limit,
      clientParams.parameters);
  auto d6dBasePMTU = getIntegerParameter(
      static_cast<TransportParameterId>(kD6DBasePMTUParameterId),
      clientParams.parameters);
  auto d6dRaiseTimeout = getIntegerParameter(
      static_cast<TransportParameterId>(kD6DRaiseTimeoutParameterId),
      clientParams.parameters);
  auto d6dProbeTimeout = getIntegerParameter(
      static_cast<TransportParameterId>(kD6DProbeTimeoutParameterId),
      clientParams.parameters);
  auto minAckDelay = getIntegerParameter(
      TransportParameterId::min_ack_delay, clientParams.parameters);
  if (conn.version == QuicVersion::QUIC_DRAFT) {
    auto initialSourceConnId = getConnIdParameter(
        TransportParameterId::initial_source_connection_id,
        clientParams.parameters);
    if (!initialSourceConnId ||
        initialSourceConnId.value() !=
            conn.readCodec->getClientConnectionId()) {
      throw QuicTransportException(
          "Initial CID does not match.",
          TransportErrorCode::TRANSPORT_PARAMETER_ERROR);
    }
  }

  // TODO Validate active_connection_id_limit
  if (packetSize && *packetSize < kMinMaxUDPPayload) {
    throw QuicTransportException(
        folly::to<std::string>(
            "Max packet size too small. received max_packetSize = ",
            *packetSize),
        TransportErrorCode::TRANSPORT_PARAMETER_ERROR);
  }

  VLOG(10) << "Client advertised flow control ";
  VLOG(10) << "conn=" << maxData.value_or(0);
  VLOG(10) << " stream bidi local=" << maxStreamDataBidiLocal.value_or(0)
           << " ";
  VLOG(10) << " stream bidi remote=" << maxStreamDataBidiRemote.value_or(0)
           << " ";
  VLOG(10) << " stream uni=" << maxStreamDataUni.value_or(0) << " ";
  VLOG(10) << conn;
  conn.flowControlState.peerAdvertisedMaxOffset = maxData.value_or(0);
  conn.flowControlState.peerAdvertisedInitialMaxStreamOffsetBidiLocal =
      maxStreamDataBidiLocal.value_or(0);
  conn.flowControlState.peerAdvertisedInitialMaxStreamOffsetBidiRemote =
      maxStreamDataBidiRemote.value_or(0);
  conn.flowControlState.peerAdvertisedInitialMaxStreamOffsetUni =
      maxStreamDataUni.value_or(0);
  conn.streamManager->setMaxLocalBidirectionalStreams(
      maxStreamsBidi.value_or(0));
  conn.streamManager->setMaxLocalUnidirectionalStreams(
      maxStreamsUni.value_or(0));
  conn.peerIdleTimeout = std::chrono::milliseconds(idleTimeout.value_or(0));
  conn.peerIdleTimeout = timeMin(conn.peerIdleTimeout, kMaxIdleTimeout);
  if (ackDelayExponent && *ackDelayExponent > kMaxAckDelayExponent) {
    throw QuicTransportException(
        "ack_delay_exponent too large",
        TransportErrorCode::TRANSPORT_PARAMETER_ERROR);
  }
  conn.peerAckDelayExponent =
      ackDelayExponent.value_or(kDefaultAckDelayExponent);
  if (minAckDelay.hasValue()) {
    conn.peerMinAckDelay = std::chrono::microseconds(minAckDelay.value());
  }

  // Default to max because we can probe PMTU now, and this will be the upper
  // limit
  uint64_t maxUdpPayloadSize = kDefaultMaxUDPPayload;
  if (packetSize) {
    maxUdpPayloadSize = std::min(*packetSize, maxUdpPayloadSize);
    conn.peerMaxUdpPayloadSize = maxUdpPayloadSize;
    if (conn.transportSettings.canIgnorePathMTU) {
      if (*packetSize > kDefaultMaxUDPPayload) {
        // A good peer should never set oversized limit, so to be safe we
        // fallback to default
        conn.udpSendPacketLen = kDefaultUDPSendPacketLen;
      } else {
        // Otherwise, canIgnorePathMTU forces us to immediately set
        // udpSendPacketLen
        // TODO: rename "canIgnorePathMTU" to "forciblySetPathMTU"
        conn.udpSendPacketLen = maxUdpPayloadSize;
      }
    }
  }

  conn.peerActiveConnectionIdLimit =
      activeConnectionIdLimit.value_or(kDefaultActiveConnectionIdLimit);

  if (partialReliability && *partialReliability != 0 &&
      conn.transportSettings.partialReliabilityEnabled) {
    conn.partialReliabilityEnabled = true;
  }
  VLOG(10) << "conn.partialReliabilityEnabled="
           << conn.partialReliabilityEnabled;

  if (conn.transportSettings.d6dConfig.enabled) {
    // Sanity check
    if (d6dBasePMTU) {
      if (*d6dBasePMTU >= kMinMaxUDPPayload &&
          *d6dBasePMTU <= kDefaultMaxUDPPayload) {
        // The reason to take the max is because we don't want d6d to send
        // probes with a smaller packet size than udpSendPacketLen, which would
        // be useless and cause meaningless delay on finding the upper bound.
        conn.d6d.basePMTU = std::max(*d6dBasePMTU, conn.udpSendPacketLen);
        conn.d6d.maxPMTU = maxUdpPayloadSize;
        VLOG(10) << "conn.d6d.basePMTU=" << conn.d6d.basePMTU;

        // Start from base
        conn.d6d.state = D6DMachineState::BASE;
        conn.d6d.meta.lastNonSearchState = D6DMachineState::DISABLED;
        conn.d6d.meta.timeLastNonSearchState = Clock::now();

        // Temporary, should be removed after transport knob pipeline works
        conn.d6d.noBlackholeDetection = true;
      } else {
        LOG(ERROR) << "client d6dBasePMTU fails sanity check: " << *d6dBasePMTU;
        // We treat base pmtu transport param as client's swich to activate d6d,
        // so not receiving that means there's no need to configure the rest d6d
        // params
        return;
      }
    }

    if (d6dRaiseTimeout) {
      if (*d6dRaiseTimeout >= kMinD6DRaiseTimeout.count()) {
        conn.d6d.raiseTimeout = std::chrono::seconds(*d6dRaiseTimeout);
        VLOG(10) << "conn.d6d.raiseTimeout=" << conn.d6d.raiseTimeout.count();
      } else {
        LOG(ERROR) << "client d6dRaiseTimeout fails sanity check: "
                   << *d6dRaiseTimeout;
      }
    }

    if (d6dProbeTimeout) {
      if (*d6dProbeTimeout >= kMinD6DProbeTimeout.count()) {
        conn.d6d.probeTimeout = std::chrono::seconds(*d6dProbeTimeout);
        VLOG(10) << "conn.d6d.probeTimeout=" << conn.d6d.probeTimeout.count();
      } else {
        LOG(ERROR) << "client d6dProbeTimeout fails sanity check: "
                   << *d6dProbeTimeout;
      }
    }
  }
}