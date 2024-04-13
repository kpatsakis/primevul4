TEST_F(
    QuicServerTransportTest,
    ClientNATRebindingWhilePathValidationOutstanding) {
  server->getNonConstConn().transportSettings.disableMigration = false;

  auto data = IOBuf::copyBuffer("bad data");
  auto packetData = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      2,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */));

  auto congestionController = server->getConn().congestionController.get();

  folly::SocketAddress newPeer("200.0.0.100", 23456);
  deliverData(std::move(packetData), true, &newPeer);

  EXPECT_TRUE(server->getConn().outstandingPathValidation);

  EXPECT_EQ(server->getConn().peerAddress, newPeer);
  EXPECT_EQ(server->getConn().migrationState.previousPeerAddresses.size(), 1);
  EXPECT_EQ(
      server->getConn().lossState.srtt, std::chrono::microseconds::zero());
  EXPECT_EQ(
      server->getConn().lossState.lrtt, std::chrono::microseconds::zero());
  EXPECT_EQ(
      server->getConn().lossState.rttvar, std::chrono::microseconds::zero());
  EXPECT_EQ(server->getConn().lossState.mrtt, kDefaultMinRtt);
  EXPECT_NE(server->getConn().congestionController.get(), nullptr);
  EXPECT_NE(server->getConn().congestionController.get(), congestionController);
  EXPECT_TRUE(server->getConn().migrationState.lastCongestionAndRtt);

  auto newCC = server->getConn().congestionController.get();
  folly::SocketAddress newPeer2("200.0.0.200", 12345);
  auto data2 = IOBuf::copyBuffer("bad data");
  auto packetData2 = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      2,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */));
  deliverData(std::move(packetData2), true, &newPeer2);

  EXPECT_TRUE(server->getConn().outstandingPathValidation);

  EXPECT_EQ(server->getConn().peerAddress, newPeer2);
  EXPECT_EQ(server->getConn().migrationState.previousPeerAddresses.size(), 1);
  EXPECT_EQ(
      server->getConn().lossState.srtt, std::chrono::microseconds::zero());
  EXPECT_EQ(
      server->getConn().lossState.lrtt, std::chrono::microseconds::zero());
  EXPECT_EQ(
      server->getConn().lossState.rttvar, std::chrono::microseconds::zero());
  EXPECT_EQ(server->getConn().lossState.mrtt, kDefaultMinRtt);
  EXPECT_EQ(server->getConn().congestionController.get(), newCC);
  EXPECT_TRUE(server->getConn().migrationState.lastCongestionAndRtt);
}