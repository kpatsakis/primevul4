 static inline struct nf_generic_net *generic_pernet(struct net *net)
 {
 	return &net->ct.nf_ct_proto.generic;
}
