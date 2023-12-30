use serde::{Deserialize, Serialize};

const CAW_RETAIN_ID: u32 = 0x0000;
pub const CAW_TEST: u32 = CAW_RETAIN_ID | 0x0001;

const BMS_BASE_ID: u32 = 0x1000;
pub const BMS_PING: u32 = BMS_BASE_ID | 0x0000; 
pub const BMS_PONG: u32 = BMS_BASE_ID | 0x0001; 
