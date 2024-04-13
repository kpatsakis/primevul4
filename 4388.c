TEST_P(
    QuicServerTransportAllowMigrationTest,
    ReceiveReorderedDataFromChangedPeerAddress) {
  auto data = IOBuf::copyBuffer("bad data");
  auto firstPacket = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      2,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */));
  auto secondPacket = packetToBuf(createStreamPacket(
      *clientConnectionId,
      *server->getConn().serverConnectionId,
      clientNextAppDataPacketNum++,
      6,
      *data,
      0 /* cipherOverhead */,
      0 /* largestAcked */));

  // Receive second packet first
  deliverData(std::move(secondPacket));

  auto peerAddress = server->getConn().peerAddress;

  // Receive first packet later from a different address
  folly::SocketAddress newPeer("100.101.102.103", 23456);
  deliverData(std::move(firstPacket), true, &newPeer);

  // No migration for reordered packet
  EXPECT_EQ(server->getConn().peerAddress, peerAddress);
}