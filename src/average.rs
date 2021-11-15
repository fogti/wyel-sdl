use std::time::Instant;
use std::ops;

pub struct WAverage<T> {
    pub last: T,
    pub weight: T,
}

impl<T> WAverage<T>
where
    T: Copy + ops::AddAssign + ops::Sub,
    <T as ops::Sub>::Output: ops::Div<T, Output = T>,
{
    pub fn push(&mut self, newv: T) {
        self.last += (newv - self.last) / self.weight;
    }
}

pub struct WTAverage {
    lastt: Instant,
    inner: WAverage<f64>,
}

impl WTAverage {
    pub fn new(startval: f64, weight: f64) -> WTAverage {
        WTAverage {
            lastt: Instant::now(),
            inner: WAverage {
                last: startval,
                weight,
            },
        }
    }
    pub fn push(&mut self) {
        let newt = Instant::now();
        let denumv = f64::from(newt.duration_since(self.lastt).as_millis() as u32);
        if denumv <= f64::EPSILON {
            return;
        }
        self.inner.push(1000f64 / denumv);
        self.lastt = newt;
    }
    pub fn get(&self) -> u32 {
        self.inner.last.round() as u32
    }
}
