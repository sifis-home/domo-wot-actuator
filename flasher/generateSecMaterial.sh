echo "Creating key and cert"
openssl genrsa -out data/Key.pem 2048
openssl req -out data/Shelly.csr -key data/Key.pem -new -sha256 -subj /CN=$1-$2.local
openssl x509 -req -in data/Shelly.csr -CA ca-domo.crt -CAkey ca-domo.key -CAcreateserial -out data/Cert.pem -days 14600 

