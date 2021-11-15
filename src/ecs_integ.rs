pub use crate::{borders::BorderSnapshot as Brds, direction::Direction};
use legion::{system, world::Entity, World};
pub use sdl2::rect::{Point, Rect};
use sdl2::render::Canvas;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Collision(pub bool);

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Bounce(pub bool);

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum ObjectType {
    Ship,
    Shot,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct ShotData {
    x: u32,
    y: u32,
    d: Direction,
}

impl ObjectType {
    fn get_img<'r>(
        self,
        imgs: &crate::imgs::WyelImages<'r>,
        is_alive: bool,
    ) -> &sdl2::render::Texture<'r> {
        let tmp = match self {
            ObjectType::Ship => &imgs.ship,
            ObjectType::Shot => &imgs.shot,
        };
        if is_alive {
            &tmp.normal
        } else {
            &tmp.destroyed
        }
    }
}

#[system(for_each)]
pub fn update_position(
    r: &mut Rect,
    d: &Direction,
    c: &mut Collision,
    b: &Bounce,
    #[resource] borders: Brds,
) {
    macro_rules! border_ctrl {
        ($dx:expr, $dy:expr) => {{
            if b.0 {
                r.offset($dx, $dy);
            } else {
                c.0 = true;
            }
        }};
    }
    if !c.0 {
        if r.left() == 0 {
            border_ctrl!(1, 0);
        } else if r.right() >= borders.x {
            border_ctrl!(-1, 0);
        } else if r.top() == 0 {
            border_ctrl!(0, 1);
        } else if r.bottom() >= borders.y {
            border_ctrl!(0, -1);
        } else {
            let (dx, dy) = d.to_offset();
            r.offset(dx, dy);
        }
    }
}

pub fn draw_stuff<T>(
    w: &mut World,
    canvas: &mut Canvas<T>,
    imgs: &crate::imgs::WyelImages<'_>,
    borders: Brds,
) {
    let mut to_remove = Vec::new();

    for (&e, &r, &ot, &c) in <(&Entity, &Rect, &ObjectType, &Collision)>::query().iter(&w) {
        if let Some(r2) = borders & r {
            ot.get_img(imgs, !c.0).blit(None, r2);
        }
        if c.0 {
            to_remove.push(e);
        }
    }

    for ent in to_remove.into_iter() {
        w.remove(ent);
    }
}

pub fn check_collisions(w: &mut World) {
    fn mark_as_collided(w: &mut World, e: &Entity) {
        if let Some(ee) = w.entry(*e) {
            if let Ok(ec) = ee.get_component_mut::<Collision>() {
                *ec = true;
            }
        }
    }

    let not_collided: Vec<_> = <(&Entity, &Rect, &Collision)>::query()
        .iter(&w)
        .filter(|(_, c)| !c.0)
        .map(|(&e, &r, _)| (e, r))
        .collect();
    for (i_idx, (i, i_r)) in not_collided.iter().enumerate().skip(1) {
        let mut i_coll = false;
        for (j, j_r) in &not_collided[..i_idx] {
            if i_r.has_intersection(j_r) {
                i_coll = true;
                mark_as_collided(w, j);
                continue;
            }
        }
        if i_coll {
            mark_as_collided(w, i);
        }
    }
}

pub fn calc_fire(r: Rect, d: Direction) -> ShotData {
    const DIVF: u32 = 4;
    const SPACING: u32 = 4;
    let (w, h) = (r.width(), r.height());
    let (xd, yd) = d.to_offset();
    ShotData {
        x: r.x() + w / DIVF + xd * (w / 2 + SPACING),
        y: r.y() + h / DIVF + yd * (h / 2 + SPACING),
        d,
    }
}
