#define DDEBUG 0

#include "ddebug.h"

#include "ngx_http_echo_timer.h"

#include <stdlib.h>
#include <ngx_log.h>

ngx_int_t
ngx_http_echo_timer_elapsed_variable(ngx_http_request_t *r,
        ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_echo_ctx_t     *ctx;
    ngx_msec_int_t          ms;
    u_char                  *p;
    ngx_time_t              *tp;
    size_t                  size;

    ctx = ngx_http_get_module_ctx(r, ngx_http_echo_module);
    if (ctx->timer_begin.sec == 0) {
        ctx->timer_begin.sec  = r->start_sec;
        ctx->timer_begin.msec = (ngx_msec_t) r->start_msec;
    }

    /* force the ngx timer to update */

#if defined nginx_version && (nginx_version >= 8035 \
        || (nginx_version < 8000 && nginx_version >= 7066))
    ngx_time_update();
#else
    ngx_time_update(0, 0);
#endif

    tp = ngx_timeofday();

    dd("old sec msec: %ld %d\n", ctx->timer_begin.sec, ctx->timer_begin.msec);
    dd("new sec msec: %ld %d\n", tp->sec, tp->msec);

    ms = (ngx_msec_int_t)
             ((tp->sec - ctx->timer_begin.sec) * 1000 +
              (tp->msec - ctx->timer_begin.msec));
    ms = (ms >= 0) ? ms : 0;

    size = sizeof("-9223372036854775808.000") - 1;
    p = ngx_palloc(r->pool, size);
    v->len = ngx_snprintf(p, size, "%T.%03M",
             ms / 1000, ms % 1000) - p;
    v->data = p;

    v->valid = 1;
    v->no_cacheable = 1;
    v->not_found = 0;
    return NGX_OK;
}


ngx_int_t
ngx_http_echo_exec_echo_reset_timer(ngx_http_request_t *r,
        ngx_http_echo_ctx_t *ctx)
{
    dd("Exec timer...");

    /* force the ngx timer to update */

#if defined nginx_version && (nginx_version >= 8035 \
        || (nginx_version < 8000 && nginx_version >= 7066))
    ngx_time_update();
#else
    ngx_time_update(0, 0);
#endif

    ctx->timer_begin = *ngx_timeofday();
    return NGX_OK;
}

