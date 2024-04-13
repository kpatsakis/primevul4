TEST_F(QuicServerTransportTest, SwitchServerCidsOneOtherCid) {
  auto& conn = server->getNonConstConn();
  auto originalCid = conn.clientConnectionId;
  auto secondCid =
      ConnectionIdData(ConnectionId(std::vector<uint8_t>{5, 6, 7, 8}), 2);
  conn.peerConnectionIds.push_back(secondCid);

  EXPECT_EQ(conn.retireAndSwitchPeerConnectionIds(), true);
  EXPECT_EQ(conn.peerConnectionIds.size(), 1);

  EXPECT_EQ(conn.pendingEvents.frames.size(), 1);
  auto retireFrame = conn.pendingEvents.frames[0].asRetireConnectionIdFrame();
  EXPECT_EQ(retireFrame->sequenceNumber, 0);

  auto replacedCid = conn.clientConnectionId;
  EXPECT_NE(originalCid, *replacedCid);
  EXPECT_EQ(secondCid.connId, *replacedCid);
}