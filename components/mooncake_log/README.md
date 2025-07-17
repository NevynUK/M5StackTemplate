# Mooncake Log
Just a simple wrapper for [fmt](https://github.com/fmtlib/fmt)

```cpp
mclog::info("Huh?? {} ..??? 0x{:02X}", 114514, 66);
mclog::info("{}", std::vector<int>{1, 23, 4, 5});
// [2077-03-07 11:45:14.191] [info] Huh?? 114514 ..??? 0x42
// [2077-03-07 11:45:14.191] [info] [1, 23, 4, 5]

mclog::warn("Huh??");
mclog::warn("{}", "6");
// [2077-03-07 11:45:14.191] [warn] Huh??
// [2077-03-07 11:45:14.191] [warn] 6

mclog::error("Huh??");
mclog::error("{}", "6");
// [2077-03-07 11:45:14.191] [error] Huh??
// [2077-03-07 11:45:14.191] [error] 6

mclog::set_time_tag_enable(false);
mclog::tagInfo("MyTag", "Huh??");
mclog::tagInfo("MyTag", "6");
// [info] [MyTag] Huh??
// [info] [MyTag] 6

// Register OnLog callback
mclog::add_on_log_callback([](mclog::LogLevel_t level, std::string msg) {
    fmt::println(">> level: {} msg: {}", static_cast<int>(level), msg);
});

mclog::info("?");
// [info] ?
// >> level: 0 msg: ?
```

