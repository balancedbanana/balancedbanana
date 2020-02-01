#include <communication/CommunicatorListener.h>
#include <communication/Communicator.h>
#include <communication/authenticator/Authenticator.h>
#include <cstring>

using namespace balancedbanana::communication;

std::pair<std::string, std::string> GenerateCert();

CommunicatorListener::CommunicatorListener(std::function<std::shared_ptr<MessageProcessor>()> processorfactory) {
    this->processorfactory = std::move(processorfactory);
}

void CommunicatorListener::listen(const std::function<void(std::shared_ptr<Message>)>& callback) {
    listener = std::make_shared<Net::TLSSocketListener>();
    listener->SetConnectionHandler([this](std::shared_ptr<Net::Socket> socket) {
        auto com = std::make_shared<Communicator>(socket, processorfactory());
    });
    auto address = std::make_shared<sockaddr_in6>();
	memset(address.get(), 0, sizeof(sockaddr_in6));
	address->sin6_family = AF_INET6;
	address->sin6_port = htons(8443);
	address->sin6_addr = in6addr_any;
    auto p = GenerateCert(); //balancedbanana::communication::authenticator::Authenticator::GeneratePrivatePublicKeyPair();
    listener->UseCertificate((uint8_t*)p.second.data(), (int)p.second.length(), Net::SSLFileType::PEM);
    listener->UsePrivateKey((uint8_t*)p.first.data(), (int)p.first.length(), Net::SSLFileType::PEM);
    // listener->UseCertificate("server.cert", Net::SSLFileType::PEM);
    // listener->UsePrivateKey("server.key", Net::SSLFileType::PEM);
    listenthread = listener->Listen(std::shared_ptr<sockaddr>(address, (sockaddr*)address.get()), sizeof(sockaddr_in6));
    listenthread->join();
}

/* Certificate creation. Demonstrates some certificate related
 * operations.
 */


#include <stdio.h>
#include <stdlib.h>

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif

int mkcert(X509 **x509p, EVP_PKEY *pkeyp, int bits, int serial, int days);
int add_ext(X509 *cert, int nid, char *value);

std::pair<std::string, std::string> GenerateCert()
	{
	BIO *bio_err;
	X509 *x509=NULL;

	bio_err=BIO_new_fp(stderr, BIO_NOCLOSE);

// Free structs automatically
    struct _guard
    {
        EVP_PKEY_CTX *ctx = NULL;
        EVP_PKEY *key = NULL;
        BIO *mem = NULL;
        ~_guard() {
            if(ctx) {
                EVP_PKEY_CTX_free(ctx);
            }
            if(key) {
                EVP_PKEY_free(key);
            }
            if(mem) {
                BIO_free(mem);
            }
        }
    } g;

    g.ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!g.ctx){
        throw std::runtime_error("Failed to generate private / public KeyPair (EVP_PKEY_CTX_new_id failed)");
    }
    if (EVP_PKEY_keygen_init(g.ctx) <= 0) {
        throw std::runtime_error("Failed to generate private / public KeyPair (EVP_PKEY_keygen_init failed)");
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(g.ctx, 2048) <= 0) {
        throw std::runtime_error("Failed to generate private / public KeyPair (EVP_PKEY_CTX_set_rsa_keygen_bits failed)");
    }
    /* Generate key */
    if (EVP_PKEY_keygen(g.ctx, &g.key) <= 0) {
        throw std::runtime_error("Failed to generate private / public KeyPair (EVP_PKEY_keygen failed)");
    }

	mkcert(&x509,g.key,512,0,365);
    
	RSA_print_fp(stdout, EVP_PKEY_get0_RSA(g.key),0);
	X509_print_fp(stdout,x509);

	PEM_write_PrivateKey(stdout,g.key,NULL,NULL,0,NULL, NULL);
	PEM_write_X509(stdout,x509);

	g.mem = BIO_new(BIO_s_mem());
    if(!g.mem) {
        throw std::runtime_error("Failed to generate private / public KeyPair (BIO_new failed)");
    }
    if(!PEM_write_bio_X509(g.mem, x509)) {
        throw std::runtime_error("Failed to generate private / public KeyPair (PEM_write_bio_PUBKEY failed)");
    }
    char buffer[100000];
    int length = BIO_read(g.mem, buffer, sizeof(buffer));
    std::string cert(buffer, length);
    if(!PEM_write_bio_PrivateKey(g.mem, g.key, NULL, NULL, 0, NULL, NULL)) {
        throw std::runtime_error("Failed to generate private / public KeyPair (PEM_write_bio_PrivateKey failed)");
    }
    length = BIO_read(g.mem, buffer, sizeof(buffer));
    std::string privkey(buffer, length);

	X509_free(x509);

	BIO_free(bio_err);

    return { std::move(privkey), std::move(cert) };
	}

int mkcert(X509 **x509p, EVP_PKEY *pk, int bits, int serial, int days)
	{
	X509 *x;
	X509_NAME *name=NULL;

	if ((x509p == NULL) || (*x509p == NULL))
		{
		if ((x=X509_new()) == NULL)
			goto err;
		}
	else
		x= *x509p;

	X509_set_version(x,2);
	ASN1_INTEGER_set(X509_get_serialNumber(x),serial);
	X509_gmtime_adj(X509_get_notBefore(x),0);
	X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);
	X509_set_pubkey(x,pk);

	name=X509_get_subject_name(x);

	/* This function creates and adds the entry, working out the
	 * correct string type and performing checks on its length.
	 * Normally we'd check the return value for errors...
	 */
	X509_NAME_add_entry_by_txt(name,"C",
				MBSTRING_ASC, (const unsigned char*)"DE", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name,"CN",
				MBSTRING_ASC, (const unsigned char*)"balancedbanana", -1, -1, 0);

	/* Its self signed so set the issuer name to be the same as the
 	 * subject.
	 */
	X509_set_issuer_name(x,name);

	/* Add various extensions: standard extensions */
	add_ext(x, NID_basic_constraints, "critical,CA:TRUE");
	add_ext(x, NID_key_usage, "critical,keyCertSign,cRLSign");

	add_ext(x, NID_subject_key_identifier, "hash");

	/* Some Netscape specific extensions */
	add_ext(x, NID_netscape_cert_type, "sslCA");

	add_ext(x, NID_netscape_comment, "example comment extension");


#ifdef CUSTOM_EXT
	/* Maybe even add our own extension based on existing */
	{
		int nid;
		nid = OBJ_create("1.2.3.4", "MyAlias", "My Test Alias Extension");
		X509V3_EXT_add_alias(nid, NID_netscape_comment);
		add_ext(x, nid, "example comment alias");
	}
#endif
	
	if (!X509_sign(x,pk,EVP_md5()))
		goto err;

	*x509p=x;
	return(1);
err:
	return(0);
	}

/* Add extension using V3 code: we can set the config file as NULL
 * because we wont reference any other sections.
 */

int add_ext(X509 *cert, int nid, char *value)
	{
	X509_EXTENSION *ex;
	X509V3_CTX ctx;
	/* This sets the 'context' of the extensions. */
	/* No configuration database */
	X509V3_set_ctx_nodb(&ctx);
	/* Issuer and subject certs: both the target since it is self signed,
	 * no request and no CRL
	 */
	X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
	ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
	if (!ex)
		return 0;

	X509_add_ext(cert,ex,-1);
	X509_EXTENSION_free(ex);
	return 1;
	}
	