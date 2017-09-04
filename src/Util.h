#include "Global.h"
int     base64_encode(const unsigned char* in, unsigned char* out, unsigned int in_len);
int     base64_decode(const unsigned char *in, unsigned char *out, unsigned int in_len);
void    generateSecureRandomString(char *s, const int length);
char   *ssorest_pstrcat(SSORestPluginPool *a, ...);
char   *toStringSafety(SSORestPluginPool *pool, unsigned char *str, int len);