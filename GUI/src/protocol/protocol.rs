use crate::utils::utils::Result;

use bincode::{self, Options};
use serde::{Deserialize, Serialize};
use std::{collections::HashMap, mem};

const MAGIC: [u8; 4] = ['C' as u8, 'A' as u8, 'W' as u8, 'X' as u8];
const VERSION: u16 = 101;
const PACK_HEAD_SIZE: usize =
    mem::size_of::<[u8; 4]>() + mem::size_of::<u32>() * 2 + mem::size_of::<u16>();

#[derive(Serialize, Deserialize, Debug, PartialEq)]
#[repr(C)]
pub struct PackHead {
    pub magic: [u8; 4], // 定位标记
    pub cmd: u32,       // 指令
    pub length: u32,    // 包体长度
    pub version: u16,   // 版本号
}

impl PackHead {
    pub fn new(cmd: u32, length: u32) -> PackHead {
        PackHead {
            magic: MAGIC,
            cmd,
            length,
            version: VERSION,
        }
    }

    pub fn check(&self) -> Result<()> {
        if self.magic.ne(&MAGIC) {
            return Err("magic error")?;
        }
        if self.version != VERSION {
            return Err("version error")?;
        }
        Ok(())
    }
}

type EventCallback = HashMap<u32, Box<dyn FnMut(&Vec<u8>)>>;

pub struct Pack {
    buf: Vec<u8>,
    events: EventCallback,
}

impl Pack {
    pub fn new() -> Pack {
        Pack {
            buf: Vec::with_capacity(4096),
            events: HashMap::new(),
        }
    }

    pub fn append(&mut self, buf: &mut Vec<u8>) {
        self.buf.append(buf);
    }

    pub fn register_event<F>(&mut self, cmd: u32, p: F)
    where
        F: 'static + FnMut(&Vec<u8>),
    {
        println!("register_event: {}", cmd);
        self.events.insert(cmd, Box::new(p));
    }
}

impl Pack {
    // 处理事件
    pub fn event_process<T: std::io::Read>(&mut self, device: &mut T) -> Result<()> {
        // 读取一次数据
        let mut buf = [0_u8; 1024];
        let len = device.read(buf.as_mut_slice())?;
        if len > 0 {
            self.buf.append(&mut buf[0..len].to_vec());
        } else {
            return Err("read failed")?;
        }
        while self.buf.len() > PACK_HEAD_SIZE {
            if let Some(index) = self.find_magic() {
                if index > 0 {
                    // 将Magic移动到索引0的位置
                    self.buf.drain(..index);
                    continue;
                }
                // 解包封包头，使用大端模式并设定固定长度编码（不压缩数据）
                let config = bincode::DefaultOptions::new()
                    .with_big_endian()
                    .with_fixint_encoding();
                if let Ok(head) = config.deserialize::<PackHead>(&self.buf[..PACK_HEAD_SIZE]) {
                    if let Ok(_) = head.check() {
                        if self.buf.len() < head.length as usize + PACK_HEAD_SIZE {
                            continue;
                        }
                        self.events.get_mut(&head.cmd).and_then(|f| {
                            Some(f(&self.buf
                                [PACK_HEAD_SIZE..PACK_HEAD_SIZE + head.length as usize]
                                .to_vec()))
                        });
                        self.buf.drain(..PACK_HEAD_SIZE + head.length as usize);
                        continue;
                    }
                }
            }
            self.buf.drain(..PACK_HEAD_SIZE);
        }
        Ok(())
    }

    // 定位Magic
    fn find_magic(&self) -> Option<usize> {
        if self.buf.len() > mem::size_of_val(&MAGIC) {
            for i in 0..self.buf.len() - mem::size_of_val(&MAGIC) + 1 {
                if (&self.buf[i..i + mem::size_of_val(&MAGIC)]).eq(&MAGIC) {
                    return Some(i);
                }
            }
        }
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::protocol::cmd;
    use bincode::Options;

    #[test]
    fn test_mem_align() {
        println!("PackHead: {:?} bytes", mem::size_of::<PackHead>());
    }

    #[test]
    fn test_serialize() {
        let head = PackHead {
            magic: MAGIC,
            cmd: 0,
            length: 1,
            version: 2,
        };
        let option = bincode::DefaultOptions::new()
            .with_big_endian()
            .with_fixint_encoding();
        let s = option.serialize(&head).unwrap();
        println!("serialize size: {}", s.len());
        let d: PackHead = option.deserialize(&s[..]).unwrap();
        assert!(head.magic.eq(&MAGIC));
        assert_eq!(head.cmd, d.cmd);
        assert_eq!(head.length, d.length);
        assert_eq!(head.version, d.version);
    }

    #[test]
    fn test_pack_head_size() {
        println!("PackHead size: {}", PACK_HEAD_SIZE);
    }

    #[test]
    fn test_vec() {
        let mut v = vec![1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
        v.drain(..5);
        println!("{:?}", v);
        assert_eq!(v, vec![6, 7, 8, 9, 10]);
    }

    #[test]
    fn test_find_magic() {
        let mut pack = Pack::new();
        let mut buf: Vec<u8> = vec![0, 1, 2, 3, 4, 5];
        pack.append(&mut buf);
        pack.append(&mut MAGIC.to_vec());
        println!("{:?}", pack.buf);
        println!("magic index: {:?}", pack.find_magic());
        assert_eq!(pack.find_magic(), Some(6));
    }

    #[test]
    fn test_check_pack_head() {
        let head = PackHead::new(cmd::CAW_TEST, 1024);
        if let Err(err) = head.check() {
            panic!("{}", err);
        }
        let head = PackHead {
            magic: [0_u8; 4],
            cmd: cmd::CAW_TEST,
            length: 1024,
            version: 0,
        };
        if let Ok(_) = head.check() {
            panic!("PackHead check error");
        }
    }
}
