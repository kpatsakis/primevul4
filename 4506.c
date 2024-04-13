TEST_P(QuicServerTransportAllowMigrationTest, MigrateToStaleValidatedPeer) {
  folly::SocketAddress newPeer("100.101.102.103", 23456);
  server->getNonConstConn().migrationState.previousPeerAddresses.push_back(
      newPeer);
  CongestionAndRttState state;
  state.peerAddress = newPeer;
  state.recordTime = Clock::now() - 2 * kTimeToRetainLastCongestionAndRttState;
  state.congestionController = ccFactory_->makeCongestionController(
      server->getNonConstConn(),
      server->getNonConstConn().transportSettings.defaultCongestionController);
  state.srtt = 1000us;
  state.lrtt = 2000us;
  state.rttvar = 3000us;
  state.srtt = 800us;
  server->getNonConstConn().migrationState.lastCongestionAndRtt =
      std::move(state);

  auto data = IOBuf::copyBuffer("bad data");
  auto packetData = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      2,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */));

  EXPECT_EQ(server->getConn().migrationState.previousPeerAddresses.size(), 1);

  auto peerAddress = server->getConn().peerAddress;
  auto congestionController = server->getConn().congestionController.get();
  auto srtt = server->getConn().lossState.srtt;
  auto lrtt = server->getConn().lossState.lrtt;
  auto rttvar = server->getConn().lossState.rttvar;
  auto mrtt = server->getConn().lossState.mrtt;

  deliverData(std::move(packetData), false, &newPeer);

  EXPECT_FALSE(server->getConn().pendingEvents.pathChallenge);
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
}