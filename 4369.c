  void SetUp() override {
    clientAddr = folly::SocketAddress("127.0.0.1", 1000);
    serverAddr = folly::SocketAddress("1.2.3.4", 8080);
    clientConnectionId = getTestConnectionId();
    initialDestinationConnectionId = clientConnectionId;
    // change the initialDestinationConnectionId to be different
    // to suss out bugs.
    initialDestinationConnectionId->data()[0] ^= 0x1;
    // set server chosen connId with processId = 0 and workerId = 1
    ServerConnectionIdParams params(0, 0, 1);
    auto sock =
        std::make_unique<NiceMock<folly::test::MockAsyncUDPSocket>>(&evb);
    socket = sock.get();
    EXPECT_CALL(*sock, write(_, _))
        .WillRepeatedly(Invoke([&](const SocketAddress&,
                                   const std::unique_ptr<folly::IOBuf>& buf) {
          serverWrites.push_back(buf->clone());
          return buf->computeChainDataLength();
        }));
    EXPECT_CALL(*sock, address()).WillRepeatedly(ReturnRef(serverAddr));
    supportedVersions = {QuicVersion::MVFST, QuicVersion::QUIC_DRAFT};
    serverCtx = createServerCtx();
    connIdAlgo_ = std::make_unique<DefaultConnectionIdAlgo>();
    ccFactory_ = std::make_shared<ServerCongestionControllerFactory>();
    server = std::make_shared<TestingQuicServerTransport>(
        &evb, std::move(sock), connCallback, serverCtx);
    server->setCongestionControllerFactory(ccFactory_);
    server->setCongestionControl(CongestionControlType::Cubic);
    server->setRoutingCallback(&routingCallback);
    server->setSupportedVersions(supportedVersions);
    server->setOriginalPeerAddress(clientAddr);
    server->setServerConnectionIdParams(params);
    server->getNonConstConn().transportSettings.statelessResetTokenSecret =
        getRandSecret();
    transportInfoCb_ = std::make_unique<NiceMock<MockQuicStats>>();
    server->setTransportStatsCallback(transportInfoCb_.get());
    initializeServerHandshake();
    server->getNonConstConn().handshakeLayer.reset(fakeHandshake);
    server->getNonConstConn().serverHandshakeLayer = fakeHandshake;
    // Allow ignoring path mtu for testing negotiation.
    server->getNonConstConn().transportSettings.canIgnorePathMTU =
        getCanIgnorePathMTU();
    server->getNonConstConn().transportSettings.disableMigration =
        getDisableMigration();
    server->setConnectionIdAlgo(connIdAlgo_.get());
    server->setClientConnectionId(*clientConnectionId);
    server->setClientChosenDestConnectionId(*initialDestinationConnectionId);
    VLOG(20) << __func__ << " client connId=" << clientConnectionId->hex()
             << ", server connId="
             << (server->getConn().serverConnectionId
                     ? server->getConn().serverConnectionId->hex()
                     : " (n/a)");
    server->accept();
    setupConnection();
    EXPECT_TRUE(server->idleTimeout().isScheduled());
    EXPECT_EQ(server->getConn().peerConnectionIds.size(), 1);
    EXPECT_EQ(
        *server->getConn().clientConnectionId,
        server->getConn().peerConnectionIds[0].connId);
  }