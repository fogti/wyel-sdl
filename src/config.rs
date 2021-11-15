// NOTE: omit datalocs from config, we want to leverage nix stuff
// + compile-time env! for that

use std::io::prelude::*;
use std::ops::Range;

pub struct Config {
    fps: Range<u16>,
    rps: Range<u16>,
    max_ships: u16,
    spawn_probab: u16,
    winsize: (u32, u32),
}

impl Default for Config {
    fn default() -> Config {
        Config {
            fps: 30..100,
            rps: 100..500,
            max_ships: 2,
            spawn_probab: 10,
            winsize: (640, 480),
        }
    }
}

macro_rules! make_config_entries {
    ($($x:ident => ($argc:expr, $y:expr)),* $(,)?) => {
        #[derive(Clone, Copy)]
        enum ConfigEntry {
            $($x),*,
        }
        impl ConfigEntry {
            fn argc(self) -> usize {
                use ConfigEntry::*;
                match self {
                    $($x => $argc),*,
                }
            }
        }
        impl std::str::FromStr for ConfigEntry {
            type Err = ();
            fn from_str(s: &str) -> Result<Self, ()> {
                use ConfigEntry::*;
                Ok(match s {
                    $($y => $x),*,
                    _ => return Err(()),
                })
            }
        }
    }
}

#[rustfmt::skip]
make_config_entries! {
    Fps         => (2, "fps"),
    Rps         => (2, "rps"),
    MaxShips    => (1, "max_ships"),
    SpawnProbab => (1, "spawn_probab"),
    Winsize     => (2, "winsize"),
}

impl Config {
    fn load_from_home() -> std::io::Result<Config> {
        let myhome = crate::fs::home();
        let mut cfgf = std::io::BufReader::new(std::fs::File::open(myhome.join("config"))?);
        let mut ret = Config::default();

        for line in cfgf.lines() {
            let line = line?;
            let parts = line.split_whitespace();
            let ent = match parts.next() {
                // 'datalocs' is a deprecated parameter
                None | Some("#") | Some("datalocs") => continue,

                Some(x) => {
                    if let Ok(y) = x.parse::<ConfigEntry>() {
                        y
                    } else {
                        eprintln!("wyel-sdl-config: WARNING: unknown param '{}'", x);
                        continue;
                    }
                }
            };
            let args = match parts
                .map(|i| i.parse::<u32>())
                .collect::<Result<Vec<_>, _>>()
            {
                Ok(args) if args.len() == y.argc() => args,
                Ok(_) | Err(_) => {
                    eprintln!("wyel-sdl-config: WARNING: invalid arguments for '{}'", x);
                    continue;
                }
            };
            use ConfigEntry::*;
            match ent {
                Fps => ret.fps = args[0].try_into().unwrap()..args[1].try_into().unwrap(),
                Rps => ret.rps = args[0].try_into().unwrap()..args[1].try_into().unwrap(),
                MaxShips => ret.max_ships = args[0].try_into().unwrap(),
                SpawnProbab => ret.spawn_probab = args[0].try_into().unwrap(),
                Winsize => ret.winsize = (args[0], args[1]),
            }
        }
    }

    pub fn store_to_home(&self) -> std::io::Result<()> {
        let myhome = crate::fs::home();
        let mut cfgf = std::fs::File::create(myhome.join("config"))?;
        writeln!(&mut cfgf, "fps {} {}", self.fps.start, self.fps.end)?;
        writeln!(&mut cfgf, "rps {} {}", self.rps.start, self.rps.end)?;
        writeln!(&mut cfgf, "max_ships {}", self.max_ships)?;
        writeln!(&mut cfgf, "spawn_probab {}", self.spawn_probab)?;
        writeln!(&mut cfgf, "winsize {} {}", self.winsize.0, self.winsize.1)?;
        cfgf.flush()
    }

    pub fn get() -> Config {
        if let Ok(x) = Config::load_from_home() {
            x
        } else {
            Config::default()
        }
    }
}

pub fn describe<W: std::io::Write>(mut out: W) -> std::io::Result<()> {
    writeln!(
        &mut out,
        "-- Configuration file syntax (location: {}/config) --",
        crate::fs::home()
    )?;
    writeln!(&mut out, "possible lines:")?;
    writeln!(&mut out, "  fps MIN MAX          set the inclusive minimum / maximum frames per second target values")?;
    writeln!(&mut out, "  rps MIN MAX          set the inclusive minimum / maximum rounds (moves) per second target values")?;
    writeln!(
        &mut out,
        "  max_ships COUNT      set the maximum number of AI ships"
    )?;
    writeln!(
        &mut out,
        "  spawn_probab NUM     set the AI ship spawn probability to (1 : NUM)"
    )?;
    writeln!(&mut out, "  winsize X Y          set the start window size")?;
}
