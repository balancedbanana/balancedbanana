#include <communication/CommunicatorListener.h>
#include <communication/Communicator.h>
#include <communication/authenticator/Authenticator.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>

using namespace balancedbanana::communication;

std::pair<std::string, std::string> GenerateCert();

CommunicatorListener::CommunicatorListener(std::function<std::shared_ptr<MessageProcessor>()> processorfactory) {
    this->processorfactory = std::move(processorfactory);
}

CommunicatorListener::~CommunicatorListener() {
	if(listenthread) {
		listenthread->join();
	}
}

void CommunicatorListener::listen(const std::function<void(std::shared_ptr<Communicator>)>& callback) {
    listener = std::make_shared<Net::TLSSocketListener>();
    listener->SetConnectionHandler([this, callback = callback](std::shared_ptr<Net::Socket> socket) {
        callback(std::make_shared<Communicator>(socket, processorfactory()));
    });
    auto address = std::make_shared<sockaddr_in6>();
	memset(address.get(), 0, sizeof(sockaddr_in6));
	address->sin6_family = AF_INET6;
	address->sin6_port = htons(8443);
	address->sin6_addr = in6addr_any;
    auto p = GenerateCert();
    listener->UseCertificate((uint8_t*)p.second.data(), (int)p.second.length(), Net::SSLFileType::PEM);
    listener->UsePrivateKey((uint8_t*)p.first.data(), (int)p.first.length(), Net::SSLFileType::PEM);
    if(!(listenthread = listener->Listen(std::shared_ptr<sockaddr>(address, (sockaddr*)address.get()), sizeof(sockaddr_in6)))) {
		throw std::runtime_error("Failed to listen");
	}
}

void mkcert(X509 **x509p, EVP_PKEY *pkeyp, int bits, int serial, int days);
int add_ext(X509 *cert, int nid, const char *value);

std::pair<std::string, std::string> GenerateCert() {
// Free structs automatically
    struct _guard
    {
        EVP_PKEY_CTX *ctx = NULL;
        EVP_PKEY *key = NULL;
        BIO *mem = NULL;
        BIO *bio_err = NULL;
		X509 *x509 = NULL;
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
			if(bio_err) {
                BIO_free(bio_err);
            }
            if(x509) {
                X509_free(x509);
            }
        }
    } g;

	g.bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

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

	mkcert(&g.x509,g.key,512,0,365);

	PEM_write_PrivateKey(stdout,g.key,NULL,NULL,0,NULL, NULL);
	PEM_write_X509(stdout,g.x509);

	g.mem = BIO_new(BIO_s_mem());
    if(!g.mem) {
        throw std::runtime_error("Failed to generate private / public KeyPair (BIO_new failed)");
    }
    if(!PEM_write_bio_X509(g.mem, g.x509)) {
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

    return { std::move(privkey), std::move(cert) };
}

void mkcert(X509 **x509p, EVP_PKEY *pk, int bits, int serial, int days)
	{
	// Free structs automatically
    struct _guard
    {
		X509 *x = NULL;
        ~_guard() {
            if(x) {
                X509_free(x);
            }
        }
    } g;
	X509_NAME *name=NULL;

	if ((x509p == NULL) || (*x509p == NULL))
		{
		if ((g.x=X509_new()) == NULL)
			throw std::runtime_error("Failed to create cert object");
		}
	else
		g.x= *x509p;

	X509_set_version(g.x,2);
	ASN1_INTEGER_set(X509_get_serialNumber(g.x),serial);
	X509_gmtime_adj(X509_get_notBefore(g.x),0);
	X509_gmtime_adj(X509_get_notAfter(g.x),(long)60*60*24*days);
	X509_set_pubkey(g.x,pk);

	name=X509_get_subject_name(g.x);

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
	X509_set_issuer_name(g.x,name);

	/* Add various extensions: standard extensions */
	add_ext(g.x, NID_basic_constraints, "critical,CA:TRUE");
	add_ext(g.x, NID_key_usage, "critical,keyCertSign,cRLSign,nonRepudiation,digitalSignature,keyEncipherment");

	add_ext(g.x, NID_subject_key_identifier, "hash");

	/* Some Netscape specific extensions */
	add_ext(g.x, NID_netscape_cert_type, "sslCA");

	add_ext(g.x, NID_netscape_comment, "balancedbanana");
	
	if (!X509_sign(g.x,pk,EVP_md5())) {
		throw std::runtime_error("Failed to generate private / public KeyPair (PEM_write_bio_PrivateKey failed)");
	}

	*x509p=g.x;
	g.x = NULL;
}

/* Add extension using V3 code: we can set the config file as NULL
 * because we wont reference any other sections.
 */

int add_ext(X509 *cert, int nid, const char *value) {
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
	