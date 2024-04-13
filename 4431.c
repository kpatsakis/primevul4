  virtual void setupConnection() {
    EXPECT_EQ(server->getConn().readCodec, nullptr);
    EXPECT_EQ(server->getConn().statsCallback, transportInfoCb_.get());
    setupClientReadCodec();
    recvClientHello();

    IOBufEqualTo eq;
    EXPECT_TRUE(eq(getCryptoStreamData(), IOBuf::copyBuffer("SHLO")));
    serverWrites.clear();

    EXPECT_NE(server->getConn().readCodec, nullptr);
    EXPECT_NE(server->getConn().initialWriteCipher, nullptr);
    EXPECT_NE(server->getConn().initialHeaderCipher, nullptr);
    EXPECT_NE(server->getConn().handshakeWriteCipher, nullptr);
    EXPECT_NE(server->getConn().handshakeWriteHeaderCipher, nullptr);
    EXPECT_NE(server->getConn().readCodec->getHandshakeHeaderCipher(), nullptr);

    EXPECT_FALSE(server->getConn().localConnectionError.has_value());
    EXPECT_EQ(server->getConn().version, QuicVersion::MVFST);
    EXPECT_EQ(server->getConn().serverConnIdParams->processId, 0);
    EXPECT_EQ(server->getConn().serverConnIdParams->workerId, 1);
    EXPECT_TRUE(server->getConn().serverConnectionId.has_value());
    EXPECT_EQ(server->getConn().selfConnectionIds.size(), 1);
    serverConnectionId = *server->getConn().serverConnectionId;
    EXPECT_EQ(
        server->getConn().selfConnectionIds[0].connId, serverConnectionId);
    // the crypto data should have been written in the previous loop, verify
    // that the write loop callback is not scheduled any more since we don't
    // have keys to write acks. This assumes that we will schedule crypto data
    // as soon as we can.
    EXPECT_FALSE(server->writeLooper()->isLoopCallbackScheduled());
    EXPECT_FALSE(server->readLooper()->isLoopCallbackScheduled());

    expectWriteNewSessionTicket();
    // Once oneRtt keys are available, ServerTransport must call the
    // onConnectionIdBound on its 'routingCallback'
    EXPECT_CALL(routingCallback, onConnectionIdBound(_))
        .WillOnce(Invoke([&, clientAddr = clientAddr](auto transport) {
          EXPECT_EQ(clientAddr, transport->getOriginalPeerAddress());
        }));

    EXPECT_TRUE(server->getConn().pendingEvents.frames.empty());
    EXPECT_EQ(server->getConn().nextSelfConnectionIdSequence, 1);
    recvClientFinished();

    // We need an extra pump here for some reason.
    loopForWrites();

    // Issue (kMinNumAvailableConnIds - 1) more connection ids on handshake
    // complete
    auto numNewConnIdFrames = 0;
    for (const auto& packet : server->getConn().outstandings.packets) {
      for (const auto& frame : packet.packet.frames) {
        switch (frame.type()) {
          case QuicWriteFrame::Type::QuicSimpleFrame: {
            const auto writeFrame = frame.asQuicSimpleFrame();
            if (writeFrame->type() ==
                QuicSimpleFrame::Type::NewConnectionIdFrame) {
              ++numNewConnIdFrames;
            }
            break;
          }
          default:
            break;
        }
      }
    }
    uint64_t connIdsToIssue = std::min(
                                  server->getConn().peerActiveConnectionIdLimit,
                                  kDefaultActiveConnectionIdLimit) -
        1;

    if (server->getConn().transportSettings.disableMigration ||
        (connIdsToIssue == 0)) {
      EXPECT_EQ(numNewConnIdFrames, 0);
      EXPECT_EQ(server->getConn().nextSelfConnectionIdSequence, 1);
    } else {
      EXPECT_EQ(numNewConnIdFrames, connIdsToIssue);
      EXPECT_EQ(
          server->getConn().nextSelfConnectionIdSequence, connIdsToIssue + 1);
    }

    EXPECT_NE(server->getConn().readCodec, nullptr);
    EXPECT_NE(server->getConn().oneRttWriteCipher, nullptr);
    EXPECT_NE(server->getConn().oneRttWriteHeaderCipher, nullptr);
    EXPECT_NE(server->getConn().readCodec->getOneRttHeaderCipher(), nullptr);

    EXPECT_TRUE(getCryptoStream(
                    *server->getConn().cryptoState, EncryptionLevel::Initial)
                    ->readBuffer.empty());
    EXPECT_FALSE(server->getConn().localConnectionError.has_value());
    verifyTransportParameters(kDefaultIdleTimeout);
    serverWrites.clear();

    auto& cryptoState = server->getConn().cryptoState;
    EXPECT_EQ(cryptoState->handshakeStream.retransmissionBuffer.size(), 0);
    EXPECT_EQ(cryptoState->oneRttStream.retransmissionBuffer.size(), 0);
  }