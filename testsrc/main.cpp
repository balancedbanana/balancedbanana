#include <communication/Communicator.h>

#include <iostream>
#include <openssl/evp.h>
// #include <openssl/evp.h>
#include <openssl/rsa.h>


#include <openssl/pem.h>

int main(int argc, char** argv) {
    // // BIO *bmd = BIO_new(BIO_f_md());
    // EVP_MD_CTX * evpctx = EVP_MD_CTX_new();
    // // auto r = BIO_get_md_ctx(bmd, &evpctx);
    // EVP_DigestInit(evpctx, EVP_sha512());
    // unsigned char buffer[100];
    // size_t len = 100;
    // int ret = EVP_DigestSign(evpctx, buffer, &len, (const unsigned char*)"Hallo Welt", 10);
    // EVP_MD_CTX_free(evpctx);

    EVP_MD_CTX *mdctx = NULL;
    int ret = 0;
    
    unsigned char *sig[1];
    const char msg[] = "Hallo Welt";
    *sig = NULL;
    
    /* Create the Message Digest Context */
    if(!(mdctx = EVP_MD_CTX_create())) abort();//goto err;
    
    //////////////////

    EVP_PKEY_CTX *ctx;
    EVP_PKEY *key = NULL;
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx){

    }
    if (EVP_PKEY_keygen_init(ctx) <= 0) {

    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {

    }

    /* Generate key */
    if (EVP_PKEY_keygen(ctx, &key) <= 0) {

    }

/////////////////////////////

    BIO *mem = BIO_new(BIO_s_mem());
    int count = PEM_write_bio_PUBKEY(mem, key);
    // char buffer[1000];
    // int ret3 = BIO_read(mem, buffer, 1000);
    EVP_PKEY * pkey = PEM_read_bio_PUBKEY(mem, NULL, NULL, NULL);
    int count2 = PEM_write_bio_PrivateKey(mem, key, NULL, NULL, 0, NULL, NULL);
    EVP_PKEY * prkey = PEM_read_bio_PrivateKey(mem, NULL, NULL, NULL);


    //////////////////
    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
    if(1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha512(), NULL, prkey))  abort();//goto err;
    
    /* Call update with the message */
    if(1 != EVP_DigestSignUpdate(mdctx, msg, 10/* strlen(msg) */)) abort();//goto err;
    
    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
    * signature. Length is returned in slen */
    size_t slen;
    if(1 != EVP_DigestSignFinal(mdctx, NULL, &slen)) abort();//goto err;
    /* Allocate memory for the signature based on size in slen */
    if(!(*sig = (unsigned char *)OPENSSL_malloc(sizeof(unsigned char) * (slen)))) abort();//goto err;
    /* Obtain the signature */
    if(1 != EVP_DigestSignFinal(mdctx, *sig, &slen)) abort();//goto err;


    ///////////////////////
/* Initialize `key` with a public key */
if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha512(), NULL, pkey)) abort();// goto err;

/* Initialize `key` with a public key */
if(1 != EVP_DigestVerifyUpdate(mdctx, msg, 10/* strlen(msg) */)) abort();// goto err;

if(1 == EVP_DigestVerifyFinal(mdctx, *sig, slen))
{
    /* Success */
    std::cout << "Success\n";
}
else
{
    std::cout << "Failure\n";
    /* Failure */
}

    ///////////////////////

    // PEM_read_P

    ///////////////////////
    
    /* Clean up */
    if(*sig && !ret) OPENSSL_free(*sig);
    if(mdctx) EVP_MD_CTX_destroy(mdctx);
    return 0;
}