#ifdef APACHE2DSO
extern "C" {
#include <stdio.h>
#include "apr_hash.h"
#include "ap_config.h"
#include "ap_provider.h"
#include "httpd.h"
#include "http_core.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"
};

#include <QObject>
#include <QString>

extern bool parseText(QString text, int mode);

int dbase_handler(request_rec *r)
{
    char mystring[200];
    if (!r->handler || strcmp(r->handler, "dbase-handler")) return (DECLINED);

	ap_set_content_type(r, "text/html");

	ap_rprintf(r, "1: %s<br>", r->hostname);
	ap_rprintf(r, "2: %s<br>", r->filename);
	ap_rprintf(r, "3: %s<br>", r->uri);
    ap_rprintf(r, "4: %s<br>", r->the_request);
	ap_rprintf(r, "5: %s<p> ", r->args); 

//	readData(r);

    FILE *f1 = fopen("/etc/dbase/dbase.ini","r");
    if (!f1) {
        ap_rprintf(r, "file open error.<br>");
        return OK;
    }

    while ((fgets(mystring, 200, f1)) != NULL) {
        ap_rprintf(r, "%s<br>", mystring);
    }	fclose(f1);

    ap_rprintf(r, "<br><b>Hello, world!</b>");

    parseText(QString("CLASS FormA OF FORM  ENDCLASS"),1);


    return OK;
}

void register_hooks(apr_pool_t *pool)
{
    ap_hook_handler(dbase_handler, NULL, NULL, APR_HOOK_LAST);
}

module AP_MODULE_DECLARE_DATA dbase_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    register_hooks
};
#endif
