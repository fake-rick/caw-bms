use lazy_static::*;
use serialport::SerialPort;
use slint::{ModelRc, SharedString, VecModel};
use std::sync::Mutex;
use std::{rc::Rc, time::Duration};
slint::include_modules!();

lazy_static! {
    static ref SERIAL: Mutex<Option<Box<dyn SerialPort>>> = Mutex::new(None);
}

fn get_serials() -> Rc<VecModel<SharedString>> {
    let ports = serialport::available_ports().expect("No ports found!");
    let serial_model: Rc<VecModel<SharedString>> = Rc::new(VecModel::from(vec![]));
    for p in ports {
        println!("{}", p.port_name);
        serial_model.push(SharedString::from(p.port_name));
    }
    serial_model
}

fn main() -> Result<(), slint::PlatformError> {
    let ui = AppWindow::new()?;

    let serial_model_rc = ModelRc::from(get_serials().clone());
    print!("{:?}", serial_model_rc);
    ui.global::<SerialService>().set_serials(serial_model_rc);

    let ui_handle = ui.as_weak();
    ui.global::<SerialService>().on_refresh_clicked(move || {
        let ui = ui_handle.unwrap();
        let serial_model_rc = ModelRc::from(get_serials().clone());
        ui.global::<SerialService>().set_serials(serial_model_rc);
    });
    ui.global::<SerialService>()
        .on_connect_clicked(|current_serial: SharedString| {
            println!("on_connect_clicked: {}", current_serial);
            if let Ok(port) = serialport::new(current_serial.as_str(), 115_200)
                .timeout(Duration::from_millis(10))
                .open()
            {
                println!("serial {:?} connected", port);
                *SERIAL.lock().unwrap() = Some(port);
            }
        });
    ui.run()
}
