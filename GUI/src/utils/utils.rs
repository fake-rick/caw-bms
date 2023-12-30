use bincode::{self, Options};
use serde::{Deserialize, Serialize};
use std::error::Error;

pub type Result<T> = std::result::Result<T, Box<dyn Error>>;


pub fn encode<T>(t: &T) -> Result<Vec<u8>>
where
    T: Serialize,
{
    let config = bincode::DefaultOptions::new()
        .with_big_endian()
        .with_fixint_encoding();
    Ok(config.serialize(t)?)
}

pub fn decode<T>(buf: &Vec<u8>) -> Result<T>
where
    T: for<'a> Deserialize<'a>,
{
    let config = bincode::DefaultOptions::new()
        .with_big_endian()
        .with_fixint_encoding();
    let t: T = config.deserialize(&buf[..])?;
    Ok(t)
}
