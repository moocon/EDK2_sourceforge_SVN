#!/bin/sh

cd openssl-1.0.2e
cp e_os2.h                    ../../../Include/openssl
cp crypto/crypto.h            ../../../Include/openssl
cp crypto/opensslv.h          ../../../Include/openssl
cp crypto/opensslconf.h       ../../../Include/openssl
cp crypto/ebcdic.h            ../../../Include/openssl
cp crypto/symhacks.h          ../../../Include/openssl
cp crypto/ossl_typ.h          ../../../Include/openssl
cp crypto/objects/objects.h   ../../../Include/openssl
cp crypto/objects/obj_mac.h   ../../../Include/openssl
cp crypto/md4/md4.h           ../../../Include/openssl
cp crypto/md5/md5.h           ../../../Include/openssl
cp crypto/sha/sha.h           ../../../Include/openssl
cp crypto/mdc2/mdc2.h         ../../../Include/openssl
cp crypto/hmac/hmac.h         ../../../Include/openssl
cp crypto/ripemd/ripemd.h     ../../../Include/openssl
cp crypto/whrlpool/whrlpool.h ../../../Include/openssl
cp crypto/des/des.h           ../../../Include/openssl
cp crypto/des/des_old.h       ../../../Include/openssl
cp crypto/aes/aes.h           ../../../Include/openssl
cp crypto/rc2/rc2.h           ../../../Include/openssl
cp crypto/rc4/rc4.h           ../../../Include/openssl
cp crypto/idea/idea.h         ../../../Include/openssl
cp crypto/bf/blowfish.h       ../../../Include/openssl
cp crypto/cast/cast.h         ../../../Include/openssl
cp crypto/camellia/camellia.h ../../../Include/openssl
cp crypto/seed/seed.h         ../../../Include/openssl
cp crypto/modes/modes.h       ../../../Include/openssl
cp crypto/bn/bn.h             ../../../Include/openssl
cp crypto/ec/ec.h             ../../../Include/openssl
cp crypto/rsa/rsa.h           ../../../Include/openssl
cp crypto/dsa/dsa.h           ../../../Include/openssl
cp crypto/ecdsa/ecdsa.h       ../../../Include/openssl
cp crypto/dh/dh.h             ../../../Include/openssl
cp crypto/ecdh/ecdh.h         ../../../Include/openssl
cp crypto/dso/dso.h           ../../../Include/openssl
cp crypto/engine/engine.h     ../../../Include/openssl
cp crypto/buffer/buffer.h     ../../../Include/openssl
cp crypto/bio/bio.h           ../../../Include/openssl
cp crypto/stack/stack.h       ../../../Include/openssl
cp crypto/stack/safestack.h   ../../../Include/openssl
cp crypto/lhash/lhash.h       ../../../Include/openssl
cp crypto/rand/rand.h         ../../../Include/openssl
cp crypto/err/err.h           ../../../Include/openssl
cp crypto/evp/evp.h           ../../../Include/openssl
cp crypto/asn1/asn1.h         ../../../Include/openssl
cp crypto/asn1/asn1_mac.h     ../../../Include/openssl
cp crypto/asn1/asn1t.h        ../../../Include/openssl
cp crypto/pem/pem.h           ../../../Include/openssl
cp crypto/pem/pem2.h          ../../../Include/openssl
cp crypto/x509/x509.h         ../../../Include/openssl
cp crypto/x509/x509_vfy.h     ../../../Include/openssl
cp crypto/x509v3/x509v3.h     ../../../Include/openssl
cp crypto/conf/conf.h         ../../../Include/openssl
cp crypto/conf/conf_api.h     ../../../Include/openssl
cp crypto/txt_db/txt_db.h     ../../../Include/openssl
cp crypto/pkcs7/pkcs7.h       ../../../Include/openssl
cp crypto/pkcs12/pkcs12.h     ../../../Include/openssl
cp crypto/comp/comp.h         ../../../Include/openssl
cp crypto/ocsp/ocsp.h         ../../../Include/openssl
cp crypto/ui/ui.h             ../../../Include/openssl
cp crypto/ui/ui_compat.h      ../../../Include/openssl
cp crypto/krb5/krb5_asn.h     ../../../Include/openssl
cp crypto/cms/cms.h           ../../../Include/openssl
cp crypto/pqueue/pqueue.h     ../../../Include/openssl
cp crypto/ts/ts.h             ../../../Include/openssl
cp crypto/srp/srp.h           ../../../Include/openssl
cp crypto/cmac/cmac.h         ../../../Include/openssl
cp ssl/ssl.h                  ../../../Include/openssl
cp ssl/ssl2.h                 ../../../Include/openssl
cp ssl/ssl3.h                 ../../../Include/openssl
cp ssl/ssl23.h                ../../../Include/openssl
cp ssl/tls1.h                 ../../../Include/openssl
cp ssl/dtls1.h                ../../../Include/openssl
cp ssl/kssl.h                 ../../../Include/openssl
cp ssl/srtp.h                 ../../../Include/openssl
cd ..
