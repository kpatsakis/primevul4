TEST_P(
    QuicServerTransportPendingDataTest,
    TestNoCipherProcessPendingZeroRttData) {
  server->getNonConstConn().qLogger =
      std::make_shared<quic::FileQLogger>(VantagePoint::Server);
  recvClientHello(false);
  auto data = IOBuf::copyBuffer("bad data");
  StreamId streamId = 2;
  // Write packet with zero rtt keys
  auto packetData = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      streamId,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */,
      std::make_pair(
          LongHeader::Types::ZeroRtt, server->getConn().supportedVersions[0]),
      false));
  deliverData(std::move(packetData), false);
  if (GetParam().acceptZeroRtt) {
    if (!GetParam().chloSync) {
      EXPECT_EQ(server->getConn().streamManager->streamCount(), 0);
      EXPECT_EQ(server->getConn().pendingZeroRttData->size(), 1);
      loopForWrites();
    }
    EXPECT_EQ(server->getConn().streamManager->streamCount(), 1);
    EXPECT_EQ(server->getConn().pendingZeroRttData, nullptr);
  } else {
    EXPECT_EQ(server->getConn().streamManager->streamCount(), 0);
    EXPECT_EQ(server->getConn().pendingZeroRttData->size(), 1);
  }
  EXPECT_EQ(
      server->getConn().qLogger->scid, server->getConn().serverConnectionId);
}