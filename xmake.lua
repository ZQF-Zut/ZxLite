set_project("RxLite")

set_version("0.0.1")

set_xmakever("2.8.6")

set_allowedmodes("debug", "release")
set_allowedplats("windows")
set_allowedarchs("x86", "x64")

set_languages("c++23")

set_warnings("all")
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    set_policy("build.warning", true)
elseif is_mode("release") then
    set_optimize("smallest")
    set_policy("build.optimization.lto", true)
end

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MD" or "MT")
end

set_encodings("source:utf-8")

rule("no-crt")
    on_load(function (target)
        -- disable exception and rtti
        -- target:set("exceptions", "no-cxx")
        -- target:add("cxflags", "/GR-", {tools = "cl"})
        -- https://stackoverflow.com/questions/59454174/exception-handling-in-subsystemnative
        target:add("cxflags", "/kernel", {tools = "cl"})
        target:add("cxflags", "/Zc:wchar_t", "/Zc:inline", "/Zc:forScope", "/GS-", "/Gy", {tools = "cl"})

        if target:is_plat("windows") then
            target:add("defines", "UNICODE", "_UNICODE")

            local entry = target:extraconf("rules", "no-crt", "entry")
            if entry then
                target:add("ldflags", "/ENTRY:" .. entry, {force = true})
                target:add("shflags", "/ENTRY:" .. entry, {force = true})
                target:add("ldflags", "/subsystem:console", {force = true})
                target:add("shflags", "/subsystem:console", {force = true})
            end
        end
    end)
rule_end()

target("RxLite")
    set_kind("static")
    add_files("src/RxLite/*.cpp")
    add_includedirs("src", {public = true})

    add_rules("no-crt")
    add_syslinks("kernel32", "user32")

target("SampleDll")
    set_kind("shared")
    add_files("test/SampleDll/*.cpp")

    add_rules("no-crt", {entry = "DllEntryPoint"})
    add_deps("RxLite")

target("SampleSampleExe")
    set_kind("binary")
    add_files("test/SampleExe/*.cpp")

    add_rules("no-crt", {entry = "Start"})
    add_deps("RxLite")
