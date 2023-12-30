mod protocol;
mod utils;
use lazy_static::*;
use protocol::*;
use serialport::SerialPort;
use std::sync::Mutex;
use std::thread;
use std::time::Duration;
slint::include_modules!();

lazy_static! {
    static ref SERIAL: Mutex<Option<Box<dyn SerialPort>>> = Mutex::new(None);
}

fn ping_device() {
    if let Ok(ports) = serialport::available_ports() {
        for p in ports {
            if let Ok(mut port) = serialport::new(p.port_name.clone(), 115200)
                .timeout(Duration::from_millis(10))
                .open()
            {
                if let Ok(data) = pingpong::BmsPing::new(p.port_name.clone()).encode() {
                    let _ = port.write(&data);
                }
            }
        }
    }
}

// #[tokio::main]
fn main() -> Result<(), slint::PlatformError> {
    let ui = AppWindow::new()?;

    let handle_weak = ui.as_weak();
    thread::spawn(move || {
        let mut pack = Pack::new();
        pack.register_event(cmd::BMS_PONG, move |_| {
            let handle_copy = handle_weak.clone();
            let _ = slint::invoke_from_event_loop(move || {
                handle_copy
                    .unwrap()
                    .global::<AppService>()
                    .set_device_state(true)
            });
        });
        loop {
            if let Err(_) = match &mut *SERIAL.lock().unwrap() {
                Some(device) => pack.event_process(device),
                None => {
                    ping_device();
                    println!("ping device ...");
                    Ok(())
                }
            } {
                *SERIAL.lock().unwrap() = None;
            }
        }
    });

    ui.run()
}
