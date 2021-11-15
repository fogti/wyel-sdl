use sdl2::rect::Point;
use std::sync::{AtomicU32 as FieldLen, Ordering};
use rand::Rng;

pub static MAX_X: FieldLen = FieldLen::new(0);
pub static MAX_Y: FieldLen = FieldLen::new(0);

pub fn rand_point<G: Rng>(r: &mut G) -> Point {
    let x = r.gen_range(0..MAX_X.load(Ordering::Acquire));
    let y = r.gen_range(0..MAX_Y.load(Ordering::Acquire));
    Point::new(x, y)
}
