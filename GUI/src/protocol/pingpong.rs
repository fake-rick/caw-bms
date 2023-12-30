use serde::{Deserialize, Serialize};

use crate::utils::utils::{self, Result};

#[derive(Serialize, Deserialize, Debug, PartialEq)]
#[repr(C)]
pub struct BmsPing {
    device_name: String,
}

impl BmsPing {
    pub fn new(device_name: String) -> Self {
        BmsPing { device_name }
    }

    pub fn encode(&self) -> Result<Vec<u8>> {
        utils::encode::<BmsPing>(self)
    }
}

#[derive(Serialize, Deserialize, Debug, PartialEq)]
#[repr(C)]
pub struct BmsPong {
    device_name: String,
}

impl BmsPing {
    pub fn decode(buf: &Vec<u8>) -> Result<Self> {
        utils::decode::<BmsPing>(buf)
    }
}
