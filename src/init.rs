use log::*;

fn setup_logger() {
    #[cfg(target_arch = "wasm32")]
    {
        std::panic::set_hook(Box::new(console_error_panic_hook::hook));
        console_log::init_with_level(log::Level::Debug).expect("Couldn't initialize logger");
        debug!("Ebb: Using web logger (console_log)");
    }
    #[cfg(not(target_arch = "wasm32"))]
    {
        env_logger::init();
        debug!("Ebb: Using native logger (env_logger)");

    }

    debug!("Ebb: Logger setup complete");
}

pub fn init() {
    setup_logger();
}