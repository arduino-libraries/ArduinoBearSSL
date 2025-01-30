rm -rf BearSSL #remove old trash
git clone https://www.bearssl.org/git/BearSSL
cd BearSSL 
curl -L https://curl.se/ca/cacert.pem -o cacert.pem
make  
chmod +x build/brssl
./build/brssl ta cacert.pem > ../src/BearSSLTrustAnchors.h
cd ..
rm -rf BearSSL 