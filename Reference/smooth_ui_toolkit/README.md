# Smooth UI Toolkit

C++ UI Animation Toolkit

- Spring animation, Easing animation interpolation
- Lvgl C++ wrapper
- signal, ringbuffer ...

![](https://pic1.imgdb.cn/item/680c639c58cb8da5c8ce22d2.gif)

![](https://pic1.imgdb.cn/item/680c58b558cb8da5c8ce1f5b.gif)

![](https://pic1.imgdb.cn/item/680c58b458cb8da5c8ce1f5a.gif)

## Animate

Basic animation interpolation class, configurable start/end, loop mode, count, animation type, etc. Default animation type is **spring**

Interpolation class abstraction deeply inspired by [Motion](https://motion.dev/), thanks to Motion team

![](https://pic1.imgdb.cn/item/680c58b458cb8da5c8ce1f57.gif)

```cpp
Animate animation;

animation.start = 200;
animation.end = 600;
animation.repeat = -1;
animation.repeatType = animate_repeat_type::reverse;

// Calling spring option here makes animation type spring
animation.springOptions().bounce = 0.4;
animation.springOptions().visualDuration = 0.6;

// If you want easing animation, just call easing option
// animation.easingOptions().easingFunction = ease::ease_out_quad;
// animation.easingOptions().duration = 0.3;

animation.init();
animation.play();

while (1) {
    // Update
    animation.update();
    // Get value
    draw_ball(animation.value(), 233);
}
```

## AnimateValue

Derived class of Animate, greatly simplifies assignment and value retrieval operations

Suitable for quick animation of widget coordinates, width/height and other parameters:

Video: [Introduction](https://www.bilibili.com/video/BV1RZcTegEUu)

![](https://pic1.imgdb.cn/item/680c58b458cb8da5c8ce1f58.gif)

```cpp
AnimateValue x = 100;
AnimateValue y = 225;

while (1) {
    // Automatically adapt to new target when assigning
    x = get_mouse_x();
    y = get_mouse_y();
  
    // Automatically update and type convert when getting value
    draw_ball(x, y);
});
```

## Lvgl Cpp

Goodbye👋 lv_obj_del, you can now use smart pointers to manage lvgl widgets

Pointer management reference: *https://github.com/vpaeder/lvglpp*

Uses Godot Signal-like signal-slot to simplify original event callbacks

![](https://pic1.imgdb.cn/item/680c58b458cb8da5c8ce1f59.gif)

```cpp
#include <smooth_lvgl.h>
// lvgl cpp wrapper as header only
// Project needs to satisfy #include <lvgl.h> dependency
// Current target version is v9.2.2

// Basic lvgl object
auto obj = new Container(lv_screen_active());
obj->setPos(50, 50);
obj->setSize(200, 100);

// Label
auto label = new Label(lv_screen_active());
label->setTextFont(&lv_font_montserrat_24);
label->setAlign(LV_ALIGN_CENTER);
label->setText("Hello World");

// Button
int count = 0;
auto btn = new Button(lv_screen_active());
btn->setPos(50, 200);
btn->label().setText("+1");
btn->onClick().connect([&]() {
    label->setText(fmt::format("{}", count++));
});

// Switch
auto sw = new Switch(lv_screen_active());
sw->setPos(50, 300);
sw->onValueChanged().connect([&](bool value) {
    label->setText(value ? "ON" : "OFF");
});

// Slider
auto slider = new Slider(lv_screen_active());
slider->setPos(50, 400);
slider->onValueChanged().connect([&](int value) {
    label->setText(fmt::format("{}", value));
});
```

## UI HAL

Animation updates use system time as reference benchmark. The related functions used come from internal definitions:

```cpp
namespace ui_hal {

/**
 * @brief Get the number of milliseconds since running
 *
 * @return uint32_t
 */
uint32_t get_tick();

/**
 * @brief Wait a specified number of milliseconds before returning
 *
 * @param ms
 */
void delay(uint32_t ms);

} // namespace ui_hal
```

The default implementation uses cpp chrono library

If needed, you can customize the implementation:

```cpp
// Arduino example, performance should be better than chrono

ui_hal::on_get_tick_ms([]() {
    return millis();
});

ui_hal::on_delay([](uint32_t ms) {
    delay(ms);
});
```

## Building Examples

Examples use [lvgl](https://github.com/lvgl/lvgl) and [raylib](https://github.com/raysan5/raylib) as graphics libraries, so you need to install [SDL2](https://github.com/libsdl-org/SDL)

### Toolchain Installation:

- **macOS:**  `brew install sdl2 cmake make`  
- **Ubuntu:**   `sudo apt install build-essential cmake libsdl2-dev`  

### Clone Project:

```bash
git clone https://github.com/Forairaaaaa/smooth_ui_toolkit.git
```

### Build:

```bash
cd smooth_ui_toolkit && mkdir build
```

```bash
cd build && cmake .. && make -j8
```

During cmake process, dependent git repositories will be pulled, ensure normal network access

## Library Integration

### CMake Project

Add to project `CMakeLists.txt`:

```cmake
# Don't build examples
set(SMOOTH_UI_TOOLKIT_BUILD_EXAMPLE OFF)

# Include library path
add_subdirectory(path/to/smooth_ui_toolkit)

# link
target_link_libraries(your_project PUBLIC
    smooth_ui_toolkit
)
```

### IDF Project

Clone repository, put it directly in `components` directory

### PIO Project

Clone repository, put it directly in `libs` directory

### Arduino Project

Clone repository, put it directly in `xxx` directory (I can't remember what that library directory is called)

## TODO

- [ ] Add macro definitions for ui_hal internal cpp implementation to avoid redundant linkage in custom implementations
- [ ] NumberFlow class has bugs on linux, DigitFlow works normally
- [ ] If AnimateValue sets delay, it only works the first time, invalid after retarget
- [ ] Scripted lvgl widget api wrapper

