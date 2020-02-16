#include <communication/authenticator/AuthHandler.h>
#include <memory>
#include <stdexcept>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace balancedbanana::communication::authenticator;
using IUser = balancedbanana::scheduler::IUser;

void AuthHandler::publickeyauthenticate(const std::shared_ptr<IUser>& user, const std::string& signature) {
    // Free structs automatically
    struct _guard
    {
        EVP_MD_CTX *mdctx = NULL;
        EVP_PKEY *key = NULL;
        BIO *mem = NULL;
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
        }
    } g;

    if(!(g.mem = BIO_new_mem_buf(user->pubkey().data(), user->pubkey().length()))){
        throw std::runtime_error("Failed to verify signature (BIO_new_mem_buf failed)");
    }

    if(!(g.key = PEM_read_bio_PUBKEY(g.mem, NULL, NULL, NULL))) {
        throw std::invalid_argument("Invalid public key!");
    }

    /* Create the Message Digest Context */
    if(!(g.mdctx = EVP_MD_CTX_create())) {
        throw std::runtime_error("Failed to verify signature (EVP_MD_CTX_create failed)");
    }

    /* Initialize `key` with a public key */
    if(1 != EVP_DigestVerifyInit(g.mdctx, NULL, EVP_sha512(), NULL, g.key)) abort();// goto err;

    /* Initialize `key` with a public key */
    if(1 != EVP_DigestVerifyUpdate(g.mdctx, user->name().data(), user->name().length())) {
        throw std::runtime_error("Authentication failed invalid signature");
    }

    if(1 != EVP_DigestVerifyFinal(g.mdctx, (const unsigned char *)signature.data(), signature.length())) {
        throw std::runtime_error("Authentication failed invalid signature");
    }
}
