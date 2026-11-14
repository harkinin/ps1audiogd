#!/usr/bin/env python
import os

os.makedirs("addons/ps1_audio_effect/bin", exist_ok=True)

# Build the official Godot C++ bindings for the Godot 4.7 extension API.
env = SConscript("godot-cpp/SConstruct", {"api_version": "4.7"})
env.Append(CPPPATH=["src"])

target = "addons/ps1_audio_effect/bin/ps1_audio_effect" + env["suffix"]
sources = Glob("src/*.cpp")
library = env.SharedLibrary(target=target, source=sources)
Default(library)
