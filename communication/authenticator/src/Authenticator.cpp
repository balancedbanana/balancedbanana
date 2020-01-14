#include "Authenticator.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace balancedbanana::communication::authenticator;

std::pair<std::string, std::string> Authenticator::GeneratePrivatePublicKeyPair() {
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
    g.mem = BIO_new(BIO_s_mem());
    if(!g.mem) {
        throw std::runtime_error("Failed to generate private / public KeyPair (BIO_new failed)");
    }
    if(!PEM_write_bio_PUBKEY(g.mem, g.key)) {
        throw std::runtime_error("Failed to generate private / public KeyPair (PEM_write_bio_PUBKEY failed)");
    }
    char buffer[1000];
    int length = BIO_read(g.mem, buffer, 1000);
    std::string pubkey(buffer, length);
    if(!PEM_write_bio_PrivateKey(g.mem, g.key, NULL, NULL, 0, NULL, NULL)) {
        throw std::runtime_error("Failed to generate private / public KeyPair (PEM_write_bio_PrivateKey failed)");
    }
    length = BIO_read(g.mem, buffer, 1000);
    std::string privkey(buffer, length);
    return { std::move(privkey), std::move(pubkey) };
}

std::string Authenticator::GenerateSignature(std::string name, std::string privkey) {
    // Free structs automatically
    struct _guard
    {
        EVP_MD_CTX *mdctx = NULL;
        EVP_PKEY *key = NULL;
        BIO *mem = NULL;
        unsigned char * sig = NULL;
        ~_guard() {
            if(mdctx) {
                EVP_MD_CTX_free(mdctx);
            }
            if(key) {
                EVP_PKEY_free(key);
            }
            if(mem) {
                BIO_free(mem);
            }
            if(sig) {
                OPENSSL_free(sig);
            }
        }
    } g;

    g.mem = BIO_new_mem_buf(privkey.data(), privkey.length());

    g.key = PEM_read_bio_PrivateKey(g.mem, NULL, NULL, NULL);
    
    /* Create the Message Digest Context */
    if(!(g.mdctx = EVP_MD_CTX_create())) {
        throw std::runtime_error("Failed to generate signature (EVP_MD_CTX_create failed)");
    }

    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
    if(1 != EVP_DigestSignInit(g.mdctx, NULL, EVP_sha512(), NULL, g.key)) {
        throw std::runtime_error("Failed to generate signature (EVP_DigestSignInit failed)");
    }
    
    /* Call update with the message */
    if(1 != EVP_DigestSignUpdate(g.mdctx, name.data(), name.length())) {
        throw std::runtime_error("Failed to generate signature (EVP_DigestSignInit failed)");
    }
    
    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
    * signature. Length is returned in slen */
    size_t slen;
    if(1 != EVP_DigestSignFinal(g.mdctx, NULL, &slen)) {
        throw std::runtime_error("Failed to generate signature (EVP_DigestSignInit failed)");
    }
    /* Allocate memory for the signature based on size in slen */
    if(!(g.sig = (unsigned char *)OPENSSL_malloc(sizeof(unsigned char) * (slen)))) {
        throw std::runtime_error("Failed to generate signature (EVP_DigestSignInit failed)");
    }
    /* Obtain the signature */
    if(1 != EVP_DigestSignFinal(g.mdctx, g.sig, &slen)) {
        throw std::runtime_error("Failed to generate signature (EVP_DigestSignInit failed)");
    }
    return std::string((char*)g.sig, slen);
}