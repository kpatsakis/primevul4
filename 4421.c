void onServerReadDataFromOpen(
    QuicServerConnectionState& conn,
    ServerEvents::ReadData& readData) {
  CHECK_EQ(conn.state, ServerState::Open);
  // Don't bother parsing if the data is empty.
  if (!readData.networkData.data ||
      readData.networkData.data->computeChainDataLength() == 0) {
    return;
  }
  if (!conn.readCodec) {
    // First packet from the peer
    folly::io::Cursor cursor(readData.networkData.data.get());
    auto initialByte = cursor.readBE<uint8_t>();
    auto parsedLongHeader = parseLongHeaderInvariant(initialByte, cursor);
    if (!parsedLongHeader) {
      VLOG(4) << "Could not parse initial packet header";
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(
            0,
            QuicTransportStatsCallback::toString(
                PacketDropReason::PARSE_ERROR));
      }
      QUIC_STATS(
          conn.statsCallback, onPacketDropped, PacketDropReason::PARSE_ERROR);
      return;
    }
    QuicVersion version = parsedLongHeader->invariant.version;
    if (version == QuicVersion::VERSION_NEGOTIATION) {
      VLOG(4) << "Server droppiong VN packet";
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(
            0,
            QuicTransportStatsCallback::toString(
                PacketDropReason::INVALID_PACKET));
      }
      QUIC_STATS(
          conn.statsCallback,
          onPacketDropped,
          PacketDropReason::INVALID_PACKET);
      return;
    }

    const auto& clientConnectionId = parsedLongHeader->invariant.srcConnId;
    const auto& initialDestinationConnectionId =
        parsedLongHeader->invariant.dstConnId;

    if (initialDestinationConnectionId.size() < kDefaultConnectionIdSize) {
      VLOG(4) << "Initial connectionid too small";
      if (conn.qLogger) {
        conn.qLogger->addPacketDrop(
            0,
            QuicTransportStatsCallback::toString(
                PacketDropReason::INITIAL_CONNID_SMALL));
      }
      QUIC_STATS(
          conn.statsCallback,
          onPacketDropped,
          PacketDropReason::INITIAL_CONNID_SMALL);
      return;
    }

    CHECK(conn.connIdAlgo) << "ConnectionIdAlgo is not set.";
    CHECK(!conn.serverConnectionId.has_value());
    // serverConnIdParams must be set by the QuicServerTransport
    CHECK(conn.serverConnIdParams);

    auto newServerConnIdData = conn.createAndAddNewSelfConnId();
    CHECK(newServerConnIdData.has_value());
    conn.serverConnectionId = newServerConnIdData->connId;

    QUIC_STATS(conn.statsCallback, onStatelessReset);
    conn.serverHandshakeLayer->accept(
        std::make_shared<ServerTransportParametersExtension>(
            version,
            conn.transportSettings.advertisedInitialConnectionWindowSize,
            conn.transportSettings.advertisedInitialBidiLocalStreamWindowSize,
            conn.transportSettings.advertisedInitialBidiRemoteStreamWindowSize,
            conn.transportSettings.advertisedInitialUniStreamWindowSize,
            conn.transportSettings.advertisedInitialMaxStreamsBidi,
            conn.transportSettings.advertisedInitialMaxStreamsUni,
            conn.transportSettings.idleTimeout,
            conn.transportSettings.ackDelayExponent,
            conn.transportSettings.maxRecvPacketSize,
            conn.transportSettings.partialReliabilityEnabled,
            *newServerConnIdData->token,
            conn.serverConnectionId.value(),
            initialDestinationConnectionId));
    conn.transportParametersEncoded = true;
    const CryptoFactory& cryptoFactory =
        conn.serverHandshakeLayer->getCryptoFactory();
    conn.readCodec = std::make_unique<QuicReadCodec>(QuicNodeType::Server);
    conn.readCodec->setInitialReadCipher(cryptoFactory.getClientInitialCipher(
        initialDestinationConnectionId, version));
    conn.readCodec->setClientConnectionId(clientConnectionId);
    conn.readCodec->setServerConnectionId(*conn.serverConnectionId);
    if (conn.qLogger) {
      conn.qLogger->setScid(conn.serverConnectionId);
      conn.qLogger->setDcid(initialDestinationConnectionId);
    }
    conn.readCodec->setCodecParameters(
        CodecParameters(conn.peerAckDelayExponent, version));
    conn.initialWriteCipher = cryptoFactory.getServerInitialCipher(
        initialDestinationConnectionId, version);

    conn.readCodec->setInitialHeaderCipher(
        cryptoFactory.makeClientInitialHeaderCipher(
            initialDestinationConnectionId, version));
    conn.initialHeaderCipher = cryptoFactory.makeServerInitialHeaderCipher(
        initialDestinationConnectionId, version);
    conn.peerAddress = conn.originalPeerAddress;
  }
  BufQueue udpData;
  udpData.append(std::move(readData.networkData.data));
  for (uint16_t processedPackets = 0;
       !udpData.empty() && processedPackets < kMaxNumCoalescedPackets;
       processedPackets++) {
    size_t dataSize = udpData.chainLength();
    auto parsedPacket = conn.readCodec->parsePacket(udpData, conn.ackStates);
    size_t packetSize = dataSize - udpData.chainLength();

    switch (parsedPacket.type()) {
      case CodecResult::Type::CIPHER_UNAVAILABLE: {
        handleCipherUnavailable(
            parsedPacket.cipherUnavailable(), conn, packetSize, readData);
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

    RegularQuicPacket* regularOptional = parsedPacket.regularPacket();
    if (!regularOptional) {
      // We were unable to parse the packet, drop for now. All the drop reasons
      // should have already been logged into QLogger and QuicTrace inside the
      // previous switch-case block. We just need to update QUIC_STATS here.
      VLOG(10) << "Not able to parse QUIC packet " << conn;
      QUIC_STATS(
          conn.statsCallback, onPacketDropped, PacketDropReason::PARSE_ERROR);
      continue;
    }

    auto protectionLevel = regularOptional->header.getProtectionType();
    auto encryptionLevel = protectionTypeToEncryptionLevel(protectionLevel);

    auto packetNum = regularOptional->header.getPacketSequenceNum();
    auto packetNumberSpace = regularOptional->header.getPacketNumberSpace();

    // TODO: enforce constraints on other protection levels.
    auto& regularPacket = *regularOptional;

    bool isProtectedPacket = protectionLevel == ProtectionType::ZeroRtt ||
        protectionLevel == ProtectionType::KeyPhaseZero ||
        protectionLevel == ProtectionType::KeyPhaseOne;

    if (!isProtectedPacket) {
      for (auto& quicFrame : regularPacket.frames) {
        auto isPadding = quicFrame.asPaddingFrame();
        auto isAck = quicFrame.asReadAckFrame();
        auto isClose = quicFrame.asConnectionCloseFrame();
        auto isCrypto = quicFrame.asReadCryptoFrame();
        auto isPing = quicFrame.asPingFrame();
        // TODO: add path challenge and response
        if (!isPadding && !isAck && !isClose && !isCrypto && !isPing) {
          QUIC_STATS(
              conn.statsCallback,
              onPacketDropped,
              PacketDropReason::PROTOCOL_VIOLATION);
          if (conn.qLogger) {
            conn.qLogger->addPacketDrop(
                packetSize,
                QuicTransportStatsCallback::toString(
                    PacketDropReason::PROTOCOL_VIOLATION));
          }
          throw QuicTransportException(
              "Invalid frame", TransportErrorCode::PROTOCOL_VIOLATION);
        }
      }
    }

    CHECK(conn.clientConnectionId);
    if (conn.qLogger) {
      conn.qLogger->addPacket(regularPacket, packetSize);
    }
    // We assume that the higher layer takes care of validating that the version
    // is supported.
    if (!conn.version) {
      LongHeader* longHeader = regularPacket.header.asLong();
      if (!longHeader) {
        throw QuicTransportException(
            "Invalid packet type", TransportErrorCode::PROTOCOL_VIOLATION);
      }
      conn.version = longHeader->getVersion();
      if (conn.version == QuicVersion::MVFST_EXPERIMENTAL) {
        setExperimentalSettings(conn);
      }
    }

    if (conn.peerAddress != readData.peer) {
      if (encryptionLevel != EncryptionLevel::AppData) {
        if (conn.qLogger) {
          conn.qLogger->addPacketDrop(
              packetSize,
              QuicTransportStatsCallback::toString(
                  PacketDropReason::PEER_ADDRESS_CHANGE));
        }
        QUIC_STATS(
            conn.statsCallback,
            onPacketDropped,
            PacketDropReason::PEER_ADDRESS_CHANGE);
        throw QuicTransportException(
            "Migration not allowed during handshake",
            TransportErrorCode::INVALID_MIGRATION);
      }

      if (conn.transportSettings.disableMigration) {
        if (conn.qLogger) {
          conn.qLogger->addPacketDrop(
              packetSize,
              QuicTransportStatsCallback::toString(
                  PacketDropReason::PEER_ADDRESS_CHANGE));
        }
        QUIC_STATS(
            conn.statsCallback,
            onPacketDropped,
            PacketDropReason::PEER_ADDRESS_CHANGE);
        throw QuicTransportException(
            "Migration disabled", TransportErrorCode::INVALID_MIGRATION);
      }
    }

    auto& ackState = getAckState(conn, packetNumberSpace);
    bool outOfOrder = updateLargestReceivedPacketNum(
        ackState, packetNum, readData.networkData.receiveTimePoint);
    if (outOfOrder) {
      QUIC_STATS(conn.statsCallback, onOutOfOrderPacketReceived);
    }
    DCHECK(hasReceivedPackets(conn));

    bool pktHasRetransmittableData = false;
    bool pktHasCryptoData = false;
    bool isNonProbingPacket = false;
    bool handshakeConfirmedThisLoop = false;

    // TODO: possibly drop the packet here, but rolling back state of
    // what we've already processed is difficult.
    for (auto& quicFrame : regularPacket.frames) {
      switch (quicFrame.type()) {
        case QuicFrame::Type::ReadAckFrame: {
          VLOG(10) << "Server received ack frame packet=" << packetNum << " "
                   << conn;
          isNonProbingPacket = true;
          ReadAckFrame& ackFrame = *quicFrame.asReadAckFrame();
          processAckFrame(
              conn,
              packetNumberSpace,
              ackFrame,
              [&](const OutstandingPacket& packet,
                  const QuicWriteFrame& packetFrame,
                  const ReadAckFrame&) {
                switch (packetFrame.type()) {
                  case QuicWriteFrame::Type::WriteStreamFrame: {
                    const WriteStreamFrame& frame =
                        *packetFrame.asWriteStreamFrame();
                    VLOG(4)
                        << "Server received ack for stream=" << frame.streamId
                        << " offset=" << frame.offset << " fin=" << frame.fin
                        << " len=" << frame.len << " " << conn;
                    auto ackedStream =
                        conn.streamManager->getStream(frame.streamId);
                    if (ackedStream) {
                      sendAckSMHandler(*ackedStream, frame);
                    }
                    break;
                  }
                  case QuicWriteFrame::Type::WriteCryptoFrame: {
                    const WriteCryptoFrame& frame =
                        *packetFrame.asWriteCryptoFrame();
                    auto cryptoStream =
                        getCryptoStream(*conn.cryptoState, encryptionLevel);
                    processCryptoStreamAck(
                        *cryptoStream, frame.offset, frame.len);
                    break;
                  }
                  case QuicWriteFrame::Type::RstStreamFrame: {
                    const RstStreamFrame& frame =
                        *packetFrame.asRstStreamFrame();
                    VLOG(4) << "Server received ack for reset stream="
                            << frame.streamId << " " << conn;
                    auto stream = conn.streamManager->getStream(frame.streamId);
                    if (stream) {
                      sendRstAckSMHandler(*stream);
                    }
                    break;
                  }
                  case QuicWriteFrame::Type::WriteAckFrame: {
                    const WriteAckFrame& frame = *packetFrame.asWriteAckFrame();
                    DCHECK(!frame.ackBlocks.empty());
                    VLOG(4) << "Server received ack for largestAcked="
                            << frame.ackBlocks.front().end << " " << conn;
                    commonAckVisitorForAckFrame(ackState, frame);
                    break;
                  }
                  case QuicWriteFrame::Type::PingFrame:
                    if (!packet.metadata.isD6DProbe) {
                      conn.pendingEvents.cancelPingTimeout = true;
                    }
                    return;
                  case QuicWriteFrame::Type::QuicSimpleFrame: {
                    const QuicSimpleFrame& frame =
                        *packetFrame.asQuicSimpleFrame();
                    // ACK of HandshakeDone is a server-specific behavior.
                    if (frame.asHandshakeDoneFrame() &&
                        conn.version != QuicVersion::MVFST_D24) {
                      // Call handshakeConfirmed outside of the packet
                      // processing loop to avoid a re-entrancy.
                      handshakeConfirmedThisLoop = true;
                    }
                    break;
                  }
                  default: {
                    break;
                  }
                }
              },
              markPacketLoss,
              readData.networkData.receiveTimePoint);
          break;
        }
        case QuicFrame::Type::RstStreamFrame: {
          RstStreamFrame& frame = *quicFrame.asRstStreamFrame();
          VLOG(10) << "Server received reset stream=" << frame.streamId << " "
                   << conn;
          pktHasRetransmittableData = true;
          isNonProbingPacket = true;
          auto stream = conn.streamManager->getStream(frame.streamId);
          if (!stream) {
            break;
          }
          receiveRstStreamSMHandler(*stream, frame);
          break;
        }
        case QuicFrame::Type::ReadCryptoFrame: {
          pktHasRetransmittableData = true;
          pktHasCryptoData = true;
          isNonProbingPacket = true;
          ReadCryptoFrame& cryptoFrame = *quicFrame.asReadCryptoFrame();
          VLOG(10) << "Server received crypto data offset="
                   << cryptoFrame.offset
                   << " len=" << cryptoFrame.data->computeChainDataLength()
                   << " currentReadOffset="
                   << getCryptoStream(*conn.cryptoState, encryptionLevel)
                          ->currentReadOffset
                   << " " << conn;
          appendDataToReadBuffer(
              *getCryptoStream(*conn.cryptoState, encryptionLevel),
              StreamBuffer(
                  std::move(cryptoFrame.data), cryptoFrame.offset, false));
          break;
        }
        case QuicFrame::Type::ReadStreamFrame: {
          ReadStreamFrame& frame = *quicFrame.asReadStreamFrame();
          VLOG(10) << "Server received stream data for stream="
                   << frame.streamId << ", offset=" << frame.offset
                   << " len=" << frame.data->computeChainDataLength()
                   << " fin=" << frame.fin << " " << conn;
          pktHasRetransmittableData = true;
          isNonProbingPacket = true;
          auto stream = conn.streamManager->getStream(frame.streamId);
          // Ignore data from closed streams that we don't have the
          // state for any more.
          if (stream) {
            receiveReadStreamFrameSMHandler(*stream, std::move(frame));
          }
          break;
        }
        case QuicFrame::Type::MaxDataFrame: {
          MaxDataFrame& connWindowUpdate = *quicFrame.asMaxDataFrame();
          VLOG(10) << "Server received max data offset="
                   << connWindowUpdate.maximumData << " " << conn;
          pktHasRetransmittableData = true;
          isNonProbingPacket = true;
          handleConnWindowUpdate(conn, connWindowUpdate, packetNum);
          break;
        }
        case QuicFrame::Type::MaxStreamDataFrame: {
          MaxStreamDataFrame& streamWindowUpdate =
              *quicFrame.asMaxStreamDataFrame();
          VLOG(10) << "Server received max stream data stream="
                   << streamWindowUpdate.streamId
                   << " offset=" << streamWindowUpdate.maximumData << " "
                   << conn;
          if (isReceivingStream(conn.nodeType, streamWindowUpdate.streamId)) {
            throw QuicTransportException(
                "Received MaxStreamDataFrame for receiving stream.",
                TransportErrorCode::STREAM_STATE_ERROR);
          }
          pktHasRetransmittableData = true;
          isNonProbingPacket = true;
          auto stream =
              conn.streamManager->getStream(streamWindowUpdate.streamId);
          if (stream) {
            handleStreamWindowUpdate(
                *stream, streamWindowUpdate.maximumData, packetNum);
          }
          break;
        }
        case QuicFrame::Type::DataBlockedFrame: {
          VLOG(10) << "Server received blocked " << conn;
          pktHasRetransmittableData = true;
          isNonProbingPacket = true;
          handleConnBlocked(conn);
          break;
        }
        case QuicFrame::Type::StreamDataBlockedFrame: {
          StreamDataBlockedFrame& blocked =
              *quicFrame.asStreamDataBlockedFrame();
          VLOG(10) << "Server received blocked stream=" << blocked.streamId
                   << " " << conn;
          pktHasRetransmittableData = true;
          isNonProbingPacket = true;
          auto stream = conn.streamManager->getStream(blocked.streamId);
          if (stream) {
            handleStreamBlocked(*stream);
          }
          break;
        }
        case QuicFrame::Type::StreamsBlockedFrame: {
          StreamsBlockedFrame& blocked = *quicFrame.asStreamsBlockedFrame();
          // peer wishes to open a stream, but is unable to due to the maximum
          // stream limit set by us
          // TODO implement the handler
          isNonProbingPacket = true;
          VLOG(10) << "Server received streams blocked limit="
                   << blocked.streamLimit << ", " << conn;
          break;
        }
        case QuicFrame::Type::ConnectionCloseFrame: {
          isNonProbingPacket = true;
          ConnectionCloseFrame& connFrame = *quicFrame.asConnectionCloseFrame();
          auto errMsg = folly::to<std::string>(
              "Server closed by peer reason=", connFrame.reasonPhrase);
          VLOG(4) << errMsg << " " << conn;
          // we want to deliver app callbacks with the peer supplied error,
          // but send a NO_ERROR to the peer.
          QUIC_TRACE(recvd_close, conn, errMsg.c_str());
          if (conn.qLogger) {
            conn.qLogger->addTransportStateUpdate(getPeerClose(errMsg));
          }
          conn.peerConnectionError = std::make_pair(
              QuicErrorCode(connFrame.errorCode), std::move(errMsg));
          if (getSendConnFlowControlBytesWire(conn) == 0 &&
              conn.flowControlState.sumCurStreamBufferLen) {
            VLOG(2) << "Client gives up a flow control blocked connection";
          }
          throw QuicTransportException(
              "Peer closed", TransportErrorCode::NO_ERROR);
          break;
        }
        case QuicFrame::Type::PingFrame:
          isNonProbingPacket = true;
          // Ping isn't retransmittable data. But we would like to ack them
          // early.
          pktHasRetransmittableData = true;
          break;
        case QuicFrame::Type::PaddingFrame:
          break;
        case QuicFrame::Type::QuicSimpleFrame: {
          pktHasRetransmittableData = true;
          QuicSimpleFrame& simpleFrame = *quicFrame.asQuicSimpleFrame();
          isNonProbingPacket |= updateSimpleFrameOnPacketReceived(
              conn, simpleFrame, packetNum, readData.peer != conn.peerAddress);
          break;
        }
        default: {
          break;
        }
      }
    }

    if (handshakeConfirmedThisLoop) {
      handshakeConfirmed(conn);
    }

    // Update writable limit before processing the handshake data. This is so
    // that if we haven't decided whether or not to validate the peer, we won't
    // increase the limit.
    updateWritableByteLimitOnRecvPacket(conn);

    if (conn.peerAddress != readData.peer) {
      // TODO use new conn id, make sure the other endpoint has new conn id
      if (isNonProbingPacket) {
        if (packetNum == ackState.largestReceivedPacketNum) {
          ShortHeader* shortHeader = regularPacket.header.asShort();
          bool intentionalMigration = false;
          if (shortHeader &&
              shortHeader->getConnectionId() != conn.serverConnectionId) {
            intentionalMigration = true;
          }
          onConnectionMigration(conn, readData.peer, intentionalMigration);
        }
      } else {
        // Server will need to response with PathResponse to the new address
        // while not updating peerAddress to new address
        if (conn.qLogger) {
          conn.qLogger->addPacketDrop(
              packetSize,
              QuicTransportStatsCallback::toString(
                  PacketDropReason::PEER_ADDRESS_CHANGE));
        }
        QUIC_STATS(
            conn.statsCallback,
            onPacketDropped,
            PacketDropReason::PEER_ADDRESS_CHANGE);
        throw QuicTransportException(
            "Probing not supported yet", TransportErrorCode::INVALID_MIGRATION);
      }
    }

    // Try reading bytes off of crypto, and performing a handshake.
    auto data = readDataFromCryptoStream(
        *getCryptoStream(*conn.cryptoState, encryptionLevel));
    if (data) {
      conn.serverHandshakeLayer->doHandshake(std::move(data), encryptionLevel);

      try {
        updateHandshakeState(conn);
      } catch (...) {
        if (conn.qLogger) {
          conn.qLogger->addPacketDrop(
              packetSize,
              QuicTransportStatsCallback::toString(
                  PacketDropReason::TRANSPORT_PARAMETER_ERROR));
        }
        QUIC_STATS(
            conn.statsCallback,
            onPacketDropped,
            QuicTransportStatsCallback::PacketDropReason::
                TRANSPORT_PARAMETER_ERROR);
        throw;
      }
    }
    updateAckSendStateOnRecvPacket(
        conn,
        ackState,
        outOfOrder,
        pktHasRetransmittableData,
        pktHasCryptoData);
    if (encryptionLevel == EncryptionLevel::Handshake &&
        conn.version != QuicVersion::MVFST_D24 && conn.initialWriteCipher) {
      conn.initialWriteCipher.reset();
      conn.initialHeaderCipher.reset();
      conn.readCodec->setInitialReadCipher(nullptr);
      conn.readCodec->setInitialHeaderCipher(nullptr);
      implicitAckCryptoStream(conn, EncryptionLevel::Initial);
    }
    QUIC_STATS(conn.statsCallback, onPacketProcessed);
  }
  VLOG_IF(4, !udpData.empty())
      << "Leaving " << udpData.chainLength()
      << " bytes unprocessed after attempting to process "
      << kMaxNumCoalescedPackets << " packets.";
}