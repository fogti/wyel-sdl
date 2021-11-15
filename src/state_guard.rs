pub struct StateGuard<T> {
    r: &mut T,
    save: Option<T>,
}

impl<T> StateGuard<T>
where
    T: Clone,
{
    #[inline]
    pub fn new(r: &mut T) -> Self {
        let save = Some(r.clone());
        Self { r, save }
    }
    #[inline]
    pub fn disable(&mut self) {
        self.save = None;
    }
}

impl<T> Drop for StateGuard<T> {
    #[inline]
    fn drop(&mut self) {
        if let Some(x) = self.save.take() {
            self.r = x;
        }
    }
}
