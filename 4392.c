TEST_F(QuicServerTransportTest, ClientAddressChangeNATRebinding) {
  server->getNonConstConn().transportSettings.disableMigration = false;
  StreamId streamId = server->createBidirectionalStream().value();
  auto data1 = IOBuf::copyBuffer("Aloha");
  server->writeChain(streamId, data1->clone(), false);
  loopForWrites();
  PacketNum packetNum1 =
      getFirstOutstandingPacket(
          server->getNonConstConn(), PacketNumberSpace::AppData)
          ->packet.header.getPacketSequenceNum();
  AckBlocks acks = {{packetNum1, packetNum1}};
  auto packet1 = createAckPacket(
      server->getNonConstConn(),
      ++clientNextAppDataPacketNum,
      acks,
      PacketNumberSpace::AppData);
  deliverData(packetToBuf(packet1));

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

  folly::SocketAddress newPeer("127.0.0.100", 23456);
  deliverData(std::move(packetData), true, &newPeer);

  EXPECT_TRUE(server->getConn().outstandingPathValidation);

  EXPECT_EQ(server->getConn().peerAddress, newPeer);
  EXPECT_EQ(server->getConn().migrationState.previousPeerAddresses.size(), 1);
  EXPECT_NE(server->getConn().lossState.srtt, 0us);
  EXPECT_NE(server->getConn().lossState.lrtt, 0us);
  EXPECT_NE(server->getConn().lossState.rttvar, 0us);
  EXPECT_NE(server->getConn().lossState.mrtt, kDefaultMinRtt);
  EXPECT_EQ(server->getConn().congestionController.get(), congestionController);
  EXPECT_FALSE(server->getConn().migrationState.lastCongestionAndRtt);
}