mod average;
mod borders;
mod config;
mod direction;
use direction::Direction;
mod ecs_integ;
mod font;
mod fs;
mod imgs;
mod state_guard;
use state_guard::StateGuard;

/// `is_hit` is replaced by sdl2::rect::Rect::has_intersection

fn main() {
    println!("Hello, world!");
}
