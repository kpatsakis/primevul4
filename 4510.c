void handleCipherUnavailable(
    CipherUnavailable* originalData,
    QuicServerConnectionState& conn,
    size_t packetSize,
    ServerEvents::ReadData& readData) {
  if (!originalData->packet || originalData->packet->empty()) {
    VLOG(10) << "drop because no data " << conn;
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(packetSize, kNoData);
    }
    QUIC_TRACE(packet_drop, conn, "no_data");
    return;
  }
  if (originalData->protectionType != ProtectionType::ZeroRtt &&
      originalData->protectionType != ProtectionType::KeyPhaseZero) {
    VLOG(10) << "drop because unexpected protection level " << conn;
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(packetSize, kUnexpectedProtectionLevel);
    }
    QUIC_TRACE(packet_drop, conn, "unexpected_protection_level");
    return;
  }

  size_t combinedSize =
      (conn.pendingZeroRttData ? conn.pendingZeroRttData->size() : 0) +
      (conn.pendingOneRttData ? conn.pendingOneRttData->size() : 0);
  if (combinedSize >= conn.transportSettings.maxPacketsToBuffer) {
    VLOG(10) << "drop because max buffered " << conn;
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(packetSize, kMaxBuffered);
    }
    QUIC_TRACE(packet_drop, conn, "max_buffered");
    return;
  }

  auto& pendingData = originalData->protectionType == ProtectionType::ZeroRtt
      ? conn.pendingZeroRttData
      : conn.pendingOneRttData;
  if (pendingData) {
    QUIC_TRACE(
        packet_buffered,
        conn,
        originalData->packetNum,
        originalData->protectionType,
        packetSize);
    if (conn.qLogger) {
      conn.qLogger->addPacketBuffered(
          originalData->packetNum, originalData->protectionType, packetSize);
    }
    ServerEvents::ReadData pendingReadData;
    pendingReadData.peer = readData.peer;
    pendingReadData.networkData = NetworkDataSingle(
        std::move(originalData->packet), readData.networkData.receiveTimePoint);
    pendingData->emplace_back(std::move(pendingReadData));
    VLOG(10) << "Adding pending data to "
             << toString(originalData->protectionType)
             << " buffer size=" << pendingData->size() << " " << conn;
  } else {
    VLOG(10) << "drop because " << toString(originalData->protectionType)
             << " buffer no longer available " << conn;
    if (conn.qLogger) {
      conn.qLogger->addPacketDrop(packetSize, kBufferUnavailable);
    }
    QUIC_TRACE(packet_drop, conn, "buffer_unavailable");
  }
}