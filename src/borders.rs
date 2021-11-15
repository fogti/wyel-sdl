use rand::Rng;
use sdl2::rect::{Point, Rect};
use std::sync::{AtomicU32 as FieldLen, Ordering};

pub static MAX_X: FieldLen = FieldLen::new(0);
pub static MAX_Y: FieldLen = FieldLen::new(0);

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct BorderSnapshot {
    pub x: u32,
    pub y: u32,
}

impl BorderSnapshot {
    pub fn new() -> Self {
        Self {
            x: MAX_X.load(Ordering::Acquire),
            y: MAX_Y.load(Ordering::Acquire),
        }
    }
}

impl From<BorderSnapshot> for Rect {
    fn from(x: BorderSnapshot) -> Rect {
        Rect::new(0, 0, x.x, x.y)
    }
}

pub fn rand_point<G: Rng>(r: &mut G) -> Point {
    let BorderSnapshot { x: mx, y: mx } = BorderSnapshot::new();
    Point::new(r.gen_range(0..mx), r.gen_range(0..my))
}
