TEST_P(QuicServerTransportAllowMigrationTest, MigrateToUnvalidatedPeer) {
  auto data = IOBuf::copyBuffer("bad data");
  auto packetData = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      2,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */));

  EXPECT_EQ(server->getConn().migrationState.previousPeerAddresses.size(), 0);

  auto peerAddress = server->getConn().peerAddress;
  auto congestionController = server->getConn().congestionController.get();
  auto srtt = server->getConn().lossState.srtt;
  auto lrtt = server->getConn().lossState.lrtt;
  auto rttvar = server->getConn().lossState.rttvar;
  auto mrtt = server->getConn().lossState.mrtt;

  folly::SocketAddress newPeer("100.101.102.103", 23456);
  deliverData(std::move(packetData), false, &newPeer);

  EXPECT_TRUE(server->getConn().pendingEvents.pathChallenge);
  EXPECT_EQ(server->getConn().peerAddress, newPeer);
  EXPECT_EQ(server->getConn().migrationState.previousPeerAddresses.size(), 1);
  EXPECT_EQ(
      server->getConn().migrationState.previousPeerAddresses.back(),
      peerAddress);
  EXPECT_EQ(server->getConn().lossState.srtt, 0us);
  EXPECT_EQ(server->getConn().lossState.lrtt, 0us);
  EXPECT_EQ(server->getConn().lossState.rttvar, 0us);
  EXPECT_EQ(server->getConn().lossState.mrtt, kDefaultMinRtt);
  EXPECT_NE(server->getConn().congestionController.get(), nullptr);
  EXPECT_NE(server->getConn().congestionController.get(), congestionController);
  EXPECT_EQ(
      server->getConn().migrationState.lastCongestionAndRtt->peerAddress,
      clientAddr);
  EXPECT_EQ(
      server->getConn()
          .migrationState.lastCongestionAndRtt->congestionController.get(),
      congestionController);
  EXPECT_EQ(server->getConn().migrationState.lastCongestionAndRtt->srtt, srtt);
  EXPECT_EQ(server->getConn().migrationState.lastCongestionAndRtt->lrtt, lrtt);
  EXPECT_EQ(
      server->getConn().migrationState.lastCongestionAndRtt->rttvar, rttvar);
  EXPECT_EQ(server->getConn().migrationState.lastCongestionAndRtt->mrtt, mrtt);

  loopForWrites();
  EXPECT_FALSE(server->getConn().pendingEvents.pathChallenge);
  EXPECT_TRUE(server->getConn().outstandingPathValidation);
  EXPECT_TRUE(server->getConn().pendingEvents.schedulePathValidationTimeout);
  EXPECT_TRUE(server->pathValidationTimeout().isScheduled());

  EXPECT_TRUE(server->getConn().pathValidationLimiter != nullptr);

  ShortHeader header(
      ProtectionType::KeyPhaseZero,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++);
  RegularQuicPacketBuilder builder(
      server->getConn().udpSendPacketLen,
      std::move(header),
      0 /* largestAcked */);
  builder.encodePacketHeader();
  ASSERT_TRUE(builder.canBuildPacket());

  writeSimpleFrame(
      PathResponseFrame(server->getConn().outstandingPathValidation->pathData),
      builder);
  auto packet = std::move(builder).buildPacket();
  deliverData(packetToBuf(packet), false, &newPeer);
  EXPECT_FALSE(server->getConn().outstandingPathValidation);
  EXPECT_FALSE(server->getConn().pendingEvents.schedulePathValidationTimeout);
  EXPECT_FALSE(server->pathValidationTimeout().isScheduled());
}