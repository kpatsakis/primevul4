TEST_F(QuicServerTransportTest, SwitchServerCidsNoOtherIds) {
  auto& conn = server->getNonConstConn();

  EXPECT_EQ(conn.retireAndSwitchPeerConnectionIds(), false);
  EXPECT_EQ(conn.pendingEvents.frames.size(), 0);
  EXPECT_EQ(conn.peerConnectionIds.size(), 1);
}