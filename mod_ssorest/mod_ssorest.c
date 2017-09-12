/**
 * @file   mod_ssorest.c
 * @author Egor Lord <elord@idfconnect.com>
 *
 */

#include "Global.h"
#include "SSORestPlugin.h"
#include "Logging.h"
static void register_hooks(apr_pool_t *pool);
static int process(request_rec *r);

static void *createServerConfiguration(apr_pool_t *p, server_rec *server);
static void *mergeServerConfiguration(apr_pool_t *p, void *base, void *add);
static const char *setSSORestEnable(cmd_parms *parms, void *cfg, const char* arg);
static const char *setSSORestTrace(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestUseServerNameAsDefault(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestSendFormParameters(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestDebugEnabled(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestACOName(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestGatewayUrl(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestLocalContent(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestPluginId(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestSecretKey(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestSSOZone(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestIgnoreExt(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestIgnoreUrl(cmd_parms *parms, void *cfg, const char *arg);
static const char *setSSORestIgnoreHeaders(cmd_parms *parms, void *cfg, const char *arg);

static const command_rec moduleDirectives[] = 
{
    AP_INIT_TAKE1("SSORestEnabled", setSSORestEnable, NULL, OR_ALL, "Enable or disable mod_ssorest"),
    AP_INIT_TAKE1("SSORestTrace", setSSORestTrace, NULL, OR_ALL, "Enable or disable libcurl debug"),
    AP_INIT_TAKE1("SSORestUseServerNameAsDefault", setSSORestUseServerNameAsDefault, NULL, OR_ALL, ""),
    AP_INIT_TAKE1("SSORestSendFormParameters", setSSORestSendFormParameters, NULL, OR_ALL, ""),
    AP_INIT_TAKE1("SSORestDebugEnabled", setSSORestDebugEnabled, NULL, OR_ALL, ""),
    AP_INIT_TAKE1("SSORestACOName", setSSORestACOName, NULL, OR_ALL, ""),
    AP_INIT_TAKE1("SSORestGatewayUrl", setSSORestGatewayUrl, NULL, OR_ALL, "Gateway Location"),
    AP_INIT_TAKE1("SSORestLocalContent",setSSORestLocalContent, NULL, OR_ALL, "Gateway Location"),
    AP_INIT_TAKE1("SSORestPluginId", setSSORestPluginId, NULL, OR_ALL, ""),
    AP_INIT_TAKE1("SSORestSecretKey",setSSORestSecretKey, NULL, OR_ALL, ""),
    AP_INIT_ITERATE("SSORestSSOZone", setSSORestSSOZone, NULL, OR_ALL, ""),
    AP_INIT_ITERATE("SSORestIgnoreExt", setSSORestIgnoreExt, NULL, OR_ALL, ""),
    AP_INIT_ITERATE("SSORestIgnoreUrl", setSSORestIgnoreUrl, NULL, OR_ALL, ""),
    AP_INIT_ITERATE("SSORestIgnoreHeaders", setSSORestIgnoreHeaders, NULL, OR_ALL, ""),
    {NULL}
};

module AP_MODULE_DECLARE_DATA   ssorest_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    createServerConfiguration,
    mergeServerConfiguration,
    moduleDirectives,
    register_hooks
};


static void *createServerConfiguration(apr_pool_t *p, server_rec *server)
{
    return createPluginConfiguration(p);
}

static void *mergeServerConfiguration(apr_pool_t *p, void *base, void *add)
{
    return mergePluginConfiguration(p, base, add);
}

static const char *setSSORestEnable(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    if (!strcasecmp(arg, "on"))
        conf->isEnabled = 1;
    return NULL;
}
static const char *setSSORestTrace(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    if (!strcasecmp(arg, "on"))
        conf->isTraceEnabled = 1;
    return NULL;
}
static const char *setSSORestUseServerNameAsDefault(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    if (!strcasecmp(arg, "on"))
        conf->useServerNameAsDefault = 1;
    return NULL;
}
static const char *setSSORestSendFormParameters(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    if (!strcasecmp(arg, "on"))
        conf->sendFormParameters = 1;
    return NULL;
}
static const char *setSSORestDebugEnabled(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    if (!strcasecmp(arg, "on"))
        conf->isDebugEnabled = 1;
    return NULL;
}
static const char *setSSORestACOName(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    conf->acoName = arg;
    return NULL;
}
static const char *setSSORestGatewayUrl(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    conf->gatewayUrl = arg;
    return NULL;
}
static const char *setSSORestLocalContent(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    conf->localrootpath = arg;
    return NULL;
}
static const char *setSSORestPluginId(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    conf->pluginId = arg;
    return NULL;
}
static const char *setSSORestSecretKey(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    conf->secretKey = arg;
    return NULL;
}
static const char *setSSORestSSOZone(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    *(const char**)apr_array_push(conf->ssoZone) = arg;
    return NULL;
}
static const char *setSSORestIgnoreExt(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    if (arg[0] != '.' || strlen(arg) < 2) 
        return "SSORestIgnoreExt should be start with '.'";
    arg++;
    *(const char**)apr_array_push(conf->ignoreExt) = arg;
    return NULL;
}
static const char *setSSORestIgnoreUrl(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    *(const char**)apr_array_push(conf->ignoreUrl) = arg;
    return NULL;
}
static const char *setSSORestIgnoreHeaders(cmd_parms *parms, void *cfg, const char *arg)
{
    SSORestPluginConfigration *conf = ap_get_module_config(parms->server->module_config, &ssorest_module);
    *(const char**)apr_array_push(conf->ignoreHeaders) = arg;
    return NULL;
}
static int process(request_rec *r)
{
    SSORestPluginConfigration *conf = ap_get_module_config(r->server->module_config, &ssorest_module);

    logEmerg(r, "isEnabled: %d", conf->isEnabled);
    logEmerg(r, "isTraceEnabled: %d", conf->isTraceEnabled);
    logEmerg(r, "useServerNameAsDefault: %d", conf->useServerNameAsDefault);
    logEmerg(r, "sendFormParameters: %d", conf->sendFormParameters);
    logEmerg(r, "acoName: %s", conf->acoName);
    logEmerg(r, "gatewayUrl: %s", conf->gatewayUrl);
    logEmerg(r, "localrootpath: %s", conf->localrootpath);
    logEmerg(r, "pluginId: %s", conf->pluginId);
    logEmerg(r, "secretKey: %s", conf->secretKey);

    UINT i;
    if (conf->ssoZone != NULL )
    {
        logEmerg(r, "ssoZone[%d]", conf->ssoZone->nelts);
        for (i = 0; i < conf->ssoZone->nelts; i++)
        {
            #ifdef APACHE
                const char *s = ((const char**)conf->ssoZone->elts)[i];
            #elif NGINX
                u_char *s = ((ngx_str_t *)conf->ssoZone->elts)[i].data;
            #endif
            logEmerg(r, "ssoZone[%d]: %s", i, s);
        }
    }

    if (conf->ignoreExt != NULL )
    {
        logEmerg(r, "ignoreExt[%d]", conf->ignoreExt->nelts);
        for (i = 0; i < conf->ignoreExt->nelts; i++)
        {
            #ifdef APACHE
                const char *s = ((const char**)conf->ignoreExt->elts)[i];
            #elif NGINX
                u_char *s = ((ngx_str_t *)conf->ignoreExt->elts)[i].data;
            #endif
            logEmerg(r, "ignoreExt[%d]: %s", i, s);
        }
    }

    if (conf->ignoreUrl != NULL )
    {
        logEmerg(r, "ignoreUrl[%d]", conf->ignoreUrl->nelts);
        for (i = 0; i < conf->ignoreUrl->nelts; i++)
        {
            #ifdef APACHE
                const char *s = ((const char**)conf->ignoreUrl->elts)[i];
            #elif NGINX
                u_char *s = ((ngx_str_t *)conf->ignoreUrl->elts)[i].data;
            #endif
            logEmerg(r, "ignoreUrl[%d]: %s", i, s);
        }
}

    // processRequest(r, conf);
    
    return OK;
}

static void register_hooks(apr_pool_t *pool) 
{
    ap_hook_check_access(process, NULL, NULL, APR_HOOK_LAST, AP_AUTH_INTERNAL_PER_URI);
}


