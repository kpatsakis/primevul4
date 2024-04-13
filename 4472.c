TEST_F(QuicUnencryptedServerTransportTest, TestNotAllowedInUnencryptedPacket) {
  // This should trigger derivation of keys.
  recvClientHello();

  StreamId streamId = 4;
  auto data = IOBuf::copyBuffer("data");

  EXPECT_THROW(
      deliverData(getHandshakePacketWithFrame(
          MaxStreamDataFrame(streamId, 100),
          *clientConnectionId,
          *getInitialCipher(),
          *getInitialHeaderCipher())),
      std::runtime_error);
  EXPECT_TRUE(server->error());
}