//! STM32 RUST Example: Button
//! ========
//!

extern crate core;
use core::result::Result;

use xwrust::errno::*;
use xwrust::xwtm;
use xwrust::types::*;

use xwrust::xwos::thd::*;
use xwrust::xwos::cthd;
use xwrust::xwos::sync::sem::*;
use xwrust::xwos::pm;

use xwrust::pin;
use xwrust::xwds::soc::*;

use libc_print::std_name::println;


/// 枚举按键事件
enum BtnEvent {
    /// 单击
    Click,
    /// 长按
    LongPress,
}

const DEBOUNCING_DELAY: XwTm = xwtm::ms(20);
const LONGPRESS_CNT: XwSq = 100;

static BTNTHD: SThd<1024, XwEr> = SThd::new("BtnThd", true);
static BTNSEM: Sem = Sem::new();

extern "C" {
    static stm32xwds_soc: Soc;
}

fn get_soc() -> &'static Soc {
    unsafe {
        &stm32xwds_soc
    }
}

pub fn start() {
    println!("STM32 RUST Example: Button");
    BTNSEM.init(0, 1);

    let _ = BTNTHD.run(|_| {
        let soc = get_soc();
        let mut rc = init(soc);
        if rc < 0 {
            println!("[RustBtn] Init ... {}", rc);
            return rc;
        }
        loop {
            if cthd::shld_frz() {
                suspend();
                cthd::freeze();
                resume();
            }
            if cthd::shld_stop() {
                break;
            }
            let res = get_event(soc);
            match res {
                Ok(event) => {
                    match event {
                        BtnEvent::Click => {
                            // TODO
                        },
                        BtnEvent::LongPress => {
                            pm::suspend();
                        },
                    };
                    req_eirq(soc);
                },
                Err(err) => {
                    req_eirq(soc);
                    if err == -EINTR || err == -ETIMEDOUT {
                        // nothing();
                    } else {
                        rc = err;
                        break;
                    }
                }
            };
        }
        fini(soc);
        return rc;
    });
}

pub fn stop() {
    BTNTHD.quit();
}

fn init(soc: &Soc) -> XwEr {
    let mut rc = soc.gpio_req(GpioPort::C, pin!(13));
    if rc == XWOK {
        rc = req_eirq(soc);
    }
    rc
}

fn fini(soc: &Soc) {
    rls_eirq(soc);
    soc.gpio_rls(GpioPort::C, pin!(13));
}

fn suspend() {
}

fn resume() {
}

fn req_eirq(soc: &Soc) -> XwEr {
    soc.eirq_req(GpioPort::C, pin!(13), 13,
                 EirqFlag::RISING | EirqFlag::WKUP,
                 isr)
}

fn rls_eirq(soc: &Soc) -> XwEr {
    soc.eirq_rls(GpioPort::C, pin!(13), 13)
}

fn isr(soc: &Soc, _: XwId) {
    if pm::get_stage() < pm::PmStage::RUNNING {
        pm::resume();
    }
    rls_eirq(soc);
    BTNSEM.post();
}

fn get_event(soc: &Soc) -> Result<BtnEvent, XwEr> {
    let rc = BTNSEM.wait();
    if rc.is_err() {
        return Err(SemError::unwrap(rc));
    }
    let rc = cthd::sleep(DEBOUNCING_DELAY);
    if rc < 0 {
        return Err(rc);
    }
    let mut cnt = 0;
    loop {
        let (rc, key) = soc.gpio_input(GpioPort::C, pin!(13));
        if rc < 0 {
            return Err(rc);
        }
        cnt += 1;
        let rc = cthd::sleep(DEBOUNCING_DELAY);
        if rc < 0 {
            return Err(rc);
        }
        if key == 0 {
            break;
        }
    }
    if cnt < LONGPRESS_CNT {
        Ok(BtnEvent::Click)
    } else {
        Ok(BtnEvent::LongPress)
    }
}
