use sdl2::image::LoadTexture;
use sdl2::render::{Texture, TextureCreator};

pub struct PhasedImage<'r> {
    pub normal: Texture<'r>,
    pub destroyed: Texture<'r>,
}

pub struct WyelImages<'r> {
    ship: PhasedImage<'r>,
    shot: PhasedImage<'r>,
}

impl<'r> PhasedImage<'r> {
    pub fn new<T>(
        txtc: &'r TextureCreator<T>,
        dirpath: &camino::Utf8Path,
        name: &str,
    ) -> Result<Self, String> {
        let mut path = dirpath.join(format!("{}.bmp", name));
        let normal = txtc.load_texture(&path)?;

        path.set_file_name(format!("{}_destroyed.bmp", name));
        let destroyed = txtc.load_texture(&path)?;

        Ok(Self { normal, destroyed })
    }
}

impl<'r> WyelImages<'r> {
    pub fn new<T>(txtc: &'r TextureCreator<T>) -> Result<Self, String> {
        let dirpath: camino::Utf8PathBuf = env!("WYEL_IMAGES_DIR").into();
        Self {
            ship: PhasedImage::new(txtc, &dirpath, "ship")?,
            shot: PhasedImage::new(txtc, &dirpath, "shot")?,
        }
    }

    #[inline]
    pub fn ship(&self) -> &PhasedImage<'r> {
        &self.ship
    }
    #[inline]
    pub fn shot(&self) -> &PhasedImage<'r> {
        &self.shot
    }
}
