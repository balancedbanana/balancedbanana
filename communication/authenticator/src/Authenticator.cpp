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