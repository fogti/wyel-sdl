use rand::distribution::{Distribution, Standard};
use rand::Rng;
use sdl2::rect::Point;
use std::ops;

#[derive(Clone, Copy, Debug)]
pub enum Direction {
    Left,
    Right,
    Up,
    Down,
}

use Direction::*;

impl ops::Neg for Direction {
    type Output = Self;
    #[inline]
    fn neg(self) -> Self {
        match self {
            Left => Right,
            Right => Left,
            Up => Down,
            Down => Up,
        }
    }
}

impl Distribution<Direction> for Standard {
    fn sample<R: Rng + ?Sized>(&self, rng: &mut R) -> Direction {
        match rng.gen_range(0..=3) {
            0 => Left,
            1 => Right,
            2 => Up,
            3 => Down,
            _ => unreachable!(),
        }
    }
}

impl Direction {
    pub fn to_offset(self) -> (i32, i32) {
        match self {
            Left => (-1, 0),
            Right => (1, 0),
            Up => (0, -1),
            Down => (0, 1),
        }
    }
}
