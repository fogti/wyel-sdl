use crate::fs::static_home;
use sdl2::{pixels, rect::Rect, render, surface, ttf};

const FONTS: [&str; 7] = [
    "liberation-fonts/LiberationMono-Regular.ttf",
    "source-pro/SourceCodePro-Regular.otf",
    "dejavu/DejaVuSansMono.ttf",
    "croscorefonts/Cousine-Regular.ttf",
    "droid/DroidSansMono.ttf",
    "freefont/FreeMono.ttf",
    "corefonts/cour.ttf",
];

/// get a font instance with specific size
/// searches for suitable fonts via location-list
///
/// `ptsize` = font size
pub fn get_font<'ttf>(
    ctx: &'ttf ttf::Sdl2TtfContext,
    ptsize: u16,
) -> Option<ttf::Font<'ttf, 'static>> {
    let dirs = ["/usr/share/fonts/".to_string().into(), static_home()];

    for font in &FONTS {
        for dir in &dirs {
            let cur = dir.join(font);
            if let Ok(x) = ctx.load_font(cur, ptsize) {
                return Some(x);
            }
        }
    }
    None
}

pub fn create_texture_from_text(
    txtc: &render::TextureCreator<T>,
    font: &ttf::Font<'_, '_>,
    lines: impl Iterator<Item = impl AsRef<str>>,
    width: u32,
    height: u32,
    color: pixels::Color,
    text_space: u16,
    text_height: &mut u32,
) -> Result<render::Texture, String> {
    let mut surface = surface::Surface::from_pixelmasks(
        width,
        height,
        pixels::PixelMasks {
            bpp: 32,
            rmask: 0,
            gmask: 0,
            bmask: 0,
            amask: 0,
        },
    )?;
    let mut my_rect = Rect::new(0, 0, 0, 0);
    let text_space: i32 = text_space.into();

    for i in lines {
        let txt_surface = font.render(i).solid(color);
        my_rect.resize(txt_surface.size());
        txt_surface.blit(None, &mut surface, Some(my_rect))?;
        my_rect.offset(0, my_rect.height() + text_space);
    }

    surface.as_texture(txtc).map_err(|i| i.to_string())
}
