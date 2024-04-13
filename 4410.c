TEST_F(QuicServerTransportTest, SetOriginalPeerAddressSetsPacketSize) {
  folly::SocketAddress v4Address("0.0.0.0", 0);
  ASSERT_TRUE(v4Address.getFamily() == AF_INET);
  server->setOriginalPeerAddress(v4Address);
  EXPECT_EQ(kDefaultV4UDPSendPacketLen, server->getConn().udpSendPacketLen);

  folly::SocketAddress v6Address("::", 0);
  ASSERT_TRUE(v6Address.getFamily() == AF_INET6);
  server->setOriginalPeerAddress(v6Address);
  EXPECT_EQ(kDefaultV6UDPSendPacketLen, server->getConn().udpSendPacketLen);

  server->closeNow(folly::none);
}