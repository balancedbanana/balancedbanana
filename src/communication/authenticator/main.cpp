#define LDAP_DEPRECATED 1
#include <ldap.h>
#include <ldap_features.h>
#include <ldap_utf8.h>

int main(int argc, char** argv) {
    LDAP *ldap;
    int ret = ldap_initialize(&ldap, argv[1]);
    ldap_simple_bind_s(ldap, argv[2], argv[3]);
    // ldap_sasl_interactive_bind_s(ldap, NULL, NULL, NULL, NULL, LDAP_SASL_INTERACTIVE,
    //     (LDAP_SASL_INTERACT_PROC*)[](LDAP *ld, unsigned int flags, void *defaults, void *interact) -> int {
        
    // }, NULL);
    berval * who;
    ret = ldap_whoami_s(ldap, &who, NULL, NULL);
    return 0;
}