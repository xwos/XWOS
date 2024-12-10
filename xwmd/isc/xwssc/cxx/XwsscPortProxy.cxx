/**
 * @file
 * @brief xwisc::XwsscPortProxy
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwmd/isc/xwssc/cxx/XwsscPortProxy.hxx"

extern "C" {
#include "xwmd/isc/xwssc/mif.h"
#include "xwmd/isc/xwssc/protocol.h"
}

namespace xwisc {

/* Non-static Member */
XwsscPortProxy::XwsscPortProxy(struct xwssc * xwssc, xwu8_t port,
                               xwu8_t default_priority, xwu8_t default_qos)
        : mProxy(xwssc)
        , mPort(port)
        , mDefaultPriority(default_priority)
        , mDefaultQos(default_qos)
{
}

xwer_t XwsscPortProxy::write(const xwu8_t data[], xwsz_t * size)
{
    return xwssc_tx(mProxy, data, size, mDefaultPriority, mPort, mDefaultQos, XWTM_MAX);
}

xwer_t XwsscPortProxy::write(const xwu8_t data[], xwsz_t * size,
                             xwu8_t pri, xwu8_t qos)
{
    return xwssc_tx(mProxy, data, size, pri, mPort, qos, XWTM_MAX);
}

xwer_t XwsscPortProxy::write(const xwu8_t data[], xwsz_t * size, xwtm_t to)
{
    return xwssc_tx(mProxy, data, size, mDefaultPriority, mPort, mDefaultQos, to);
}

xwer_t XwsscPortProxy::write(const xwu8_t data[], xwsz_t * size,
                             xwu8_t pri, xwu8_t qos, xwtm_t to)
{
    return xwssc_tx(mProxy, data, size, pri, mPort, qos, to);
}

xwer_t XwsscPortProxy::asyncWrite(const xwu8_t data[], xwsz_t * size,
                                  xwssc_txh_t * txhbuf)
{
    return xwssc_eq(mProxy, data, size, mDefaultPriority, mPort, mDefaultQos,
                    sOnAsyncWrited, this, txhbuf);
}

xwer_t XwsscPortProxy::asyncWrite(const xwu8_t data[], xwsz_t * size,
                                  xwu8_t pri, xwu8_t qos,
                                  xwssc_txh_t * txhbuf)
{
    return xwssc_eq(mProxy, data, size, pri, mPort, qos,
                    sOnAsyncWrited, this, txhbuf);
}

xwer_t XwsscPortProxy::abortAsyncWrite(xwssc_txh_t txh)
{
    return xwssc_abort(mProxy, txh);
}

xwer_t XwsscPortProxy::read(xwu8_t rxbuf[], xwsz_t * size)
{
    return xwssc_rx(mProxy, mPort, rxbuf, size, nullptr, XWTM_MAX);
}

xwer_t XwsscPortProxy::read(xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos, xwtm_t to)
{
    return xwssc_rx(mProxy, mPort, rxbuf, size, qos, to);
}

xwer_t XwsscPortProxy::tryRead(xwu8_t rxbuf[], xwsz_t * size)
{
    return xwssc_try_rx(mProxy, mPort, rxbuf, size, nullptr);
}

xwer_t XwsscPortProxy::tryRead(xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos)
{
    return xwssc_try_rx(mProxy, mPort, rxbuf, size, qos);
}

void XwsscPortProxy::onAsyncWrited(xwssc_txh_t txh, xwer_t rc)
{
    XWOS_UNUSED(txh);
    XWOS_UNUSED(rc);
}

/* Static Member */
void XwsscPortProxy::sOnAsyncWrited(struct xwssc * xwssc, xwssc_txh_t txh,
                                    xwer_t rc, void * arg)
{
    XwsscPortProxy * proxy = reinterpret_cast<XwsscPortProxy *>(arg);
    XWOS_UNUSED(xwssc);
    proxy->onAsyncWrited(txh, rc);
}

} // namespace xwisc
