TEST_F(QuicServerTransportTest, TestRegisterPMTUZeroBlackholeDetection) {
  server->handleKnobParams(
      {{static_cast<uint64_t>(
            TransportKnobParamId::ZERO_PMTU_BLACKHOLE_DETECTION),
        1}});
  EXPECT_TRUE(server->getConn().d6d.noBlackholeDetection);
}