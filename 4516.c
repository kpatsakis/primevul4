void onServerReadDataFromClosed(
    QuicServerConnectionState& conn,
    ServerEvents::ReadData& readData) {
  CHECK_EQ(conn.state, ServerState::Closed);
  BufQueue udpData;
  udpData.append(std::move(readData.networkData.data));
  auto packetSize = udpData.empty() ? 0 : udpData.chainLength();
  if (!conn.readCodec) {
    // drop data. We closed before we even got the first packet. This is
    // normally not possible but might as well.
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(
          packetSize,
          QuicTransportStatsCallback::toString(
              PacketDropReason::SERVER_STATE_CLOSED));
    }
    QUIC_STATS(
        conn.statsCallback,
        onPacketDropped,
        PacketDropReason::SERVER_STATE_CLOSED);
    return;
  }

  if (conn.peerConnectionError) {
    // We already got a peer error. We can ignore any futher peer errors.
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(
          packetSize,
          QuicTransportStatsCallback::toString(
              PacketDropReason::SERVER_STATE_CLOSED));
    }
    QUIC_TRACE(packet_drop, conn, "ignoring peer close");
    QUIC_STATS(
        conn.statsCallback,
        onPacketDropped,
        PacketDropReason::SERVER_STATE_CLOSED);
    return;
  }
  auto parsedPacket = conn.readCodec->parsePacket(udpData, conn.ackStates);
  switch (parsedPacket.type()) {
    case CodecResult::Type::CIPHER_UNAVAILABLE: {
      VLOG(10) << "drop cipher unavailable " << conn;
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(packetSize, kCipherUnavailable);
      }
      QUIC_TRACE(packet_drop, conn, "cipher_unavailable");
      break;
    }
    case CodecResult::Type::RETRY: {
      VLOG(10) << "drop because the server is not supposed to "
               << "receive a retry " << conn;
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(packetSize, kRetry);
      }
      QUIC_TRACE(packet_drop, conn, "retry");
      break;
    }
    case CodecResult::Type::STATELESS_RESET: {
      VLOG(10) << "drop because reset " << conn;
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(packetSize, kReset);
      }
      QUIC_TRACE(packet_drop, conn, "reset");
      break;
    }
    case CodecResult::Type::NOTHING: {
      VLOG(10) << "drop cipher unavailable, no data " << conn;
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(packetSize, kCipherUnavailable);
      }
      QUIC_TRACE(packet_drop, conn, "cipher_unavailable");
      break;
    }
    case CodecResult::Type::REGULAR_PACKET:
      break;
  }
  auto regularOptional = parsedPacket.regularPacket();
  if (!regularOptional) {
    // We were unable to parse the packet, drop for now.
    VLOG(10) << "Not able to parse QUIC packet " << conn;
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(
          packetSize,
          QuicTransportStatsCallback::toString(PacketDropReason::PARSE_ERROR));
    }
    QUIC_STATS(
        conn.statsCallback, onPacketDropped, PacketDropReason::PARSE_ERROR);
    return;
  }

  auto& regularPacket = *regularOptional;
  auto packetNum = regularPacket.header.getPacketSequenceNum();
  auto pnSpace = regularPacket.header.getPacketNumberSpace();
  if (conn.qLogger) {
    conn.qLogger->addPacket(regularPacket, packetSize);
  }

  // Only process the close frames in the packet
  for (auto& quicFrame : regularPacket.frames) {
    switch (quicFrame.type()) {
      case QuicFrame::Type::ConnectionCloseFrame: {
        ConnectionCloseFrame& connFrame = *quicFrame.asConnectionCloseFrame();
        auto errMsg = folly::to<std::string>(
            "Server closed by peer reason=", connFrame.reasonPhrase);
        VLOG(4) << errMsg << " " << conn;
        if (conn.qLogger) {
          conn.qLogger->addTransportStateUpdate(getPeerClose(errMsg));
        }
        // we want to deliver app callbacks with the peer supplied error,
        // but send a NO_ERROR to the peer.
        QUIC_TRACE(recvd_close, conn, errMsg.c_str());
        conn.peerConnectionError = std::make_pair(
            QuicErrorCode(connFrame.errorCode), std::move(errMsg));
        break;
      }
      default:
        break;
    }
  }

  // We only need to set the largest received packet number in order to
  // determine whether or not we need to send a new close.
  auto& largestReceivedPacketNum =
      getAckState(conn, pnSpace).largestReceivedPacketNum;
  largestReceivedPacketNum = std::max<PacketNum>(
      largestReceivedPacketNum.value_or(packetNum), packetNum);
}