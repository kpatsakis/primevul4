make_fake_lchid(packet_info *pinfo _U_, gint trchld)
{
    if ( fake_map[trchld] == 0) {
        fake_map[trchld] = fakes;
        fakes++;
    }
    return fake_map[trchld];
}
