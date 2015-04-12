# Urho3D-CEF3
Urho3D CEF3 integration

Thanks to https://github.com/qwertzui11/cef_osr for being an example to learn from.

# Shutdown Crash
Currently CEF3 crashes on shutdown.
I couldn't find a way to fix it.
My attempts are commented out in the source code.

Maybe CEF1 won't crash, but for now I'm moving on.

# Color
CEF3's render buffer is BGRA and Urho3D is RGBA.
I couldn't find a way to define Urho3D's texture format to be BGRA.
A manual conversion should be possible.
