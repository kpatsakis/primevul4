TEST_F(QuicServerTransportTest, TooManyMigrations) {
  auto qLogger = std::make_shared<FileQLogger>(VantagePoint::Server);
  server->getNonConstConn().qLogger = qLogger;
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

  for (size_t i = 0; i < kMaxNumMigrationsAllowed; ++i) {
    folly::SocketAddress newPeer("100.101.102.103", 23456 + i);
    deliverData(packetData->clone(), false, &newPeer);
  }

  folly::SocketAddress newPeer("200.101.102.103", 23456);
  try {
    deliverData(packetData->clone(), false, &newPeer);
  } catch (const std::runtime_error& ex) {
    EXPECT_EQ(std::string(ex.what()), "Invalid migration");
  }
  EXPECT_TRUE(server->getConn().localConnectionError);
  EXPECT_EQ(
      server->getConn().localConnectionError->second, "Too many migrations");
  EXPECT_TRUE(server->isClosed());
  std::vector<int> indices =
      getQLogEventIndices(QLogEventType::PacketDrop, qLogger);
  EXPECT_EQ(indices.size(), 1);
  auto tmp = std::move(qLogger->logs[indices[0]]);
  auto event = dynamic_cast<QLogPacketDropEvent*>(tmp.get());
  EXPECT_EQ(event->packetSize, 0);
  EXPECT_EQ(
      event->dropReason,
      QuicTransportStatsCallback::toString(
          PacketDropReason::PEER_ADDRESS_CHANGE));
}