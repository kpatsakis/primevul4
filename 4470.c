TEST_F(QuicServerTransportTest, SwitchServerCidsMultipleCids) {
  auto& conn = server->getNonConstConn();
  auto originalCid = conn.clientConnectionId;
  auto secondCid =
      ConnectionIdData(ConnectionId(std::vector<uint8_t>{5, 6, 7, 8}), 2);
  auto thirdCid =
      ConnectionIdData(ConnectionId(std::vector<uint8_t>{3, 3, 3, 3}), 3);

  conn.peerConnectionIds.push_back(secondCid);
  conn.peerConnectionIds.push_back(thirdCid);

  EXPECT_EQ(conn.retireAndSwitchPeerConnectionIds(), true);
  EXPECT_EQ(conn.peerConnectionIds.size(), 2);

  EXPECT_EQ(conn.pendingEvents.frames.size(), 1);
  auto retireFrame = conn.pendingEvents.frames[0].asRetireConnectionIdFrame();
  EXPECT_EQ(retireFrame->sequenceNumber, 0);

  // Uses the first unused connection id.
  auto replacedCid = conn.clientConnectionId;
  EXPECT_NE(originalCid, *replacedCid);
  EXPECT_EQ(secondCid.connId, *replacedCid);
}