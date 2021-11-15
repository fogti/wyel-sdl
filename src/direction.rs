use std::ops;
use rand::distribution::{Distribution, Standard};
use rand::Rng;

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
    #[inline]
    pub fn apply<T>(self, x: &mut T, y: &mut T)
    where
        T: From<u8> + ops::AddAssign + ops::SubAssign,
    {
        match self {
            Left => x -= T::from(1),
            Right => x += T::from(1),
            Up => y -= T::from(1),
            Down => y += T::from(1),
        }
    }
}
