:: Requires shadercross CLI installed from SDL_shadercross
@echo off
setlocal enabledelayedexpansion

for %%f in (*.vert.hlsl) do (
    if exist "%%f" (
        shadercross "%%f" -o "..\compiled\spirv\%%~nf.spv"
        shadercross "%%f" -o "..\compiled\msl\%%~nf.msl"
        shadercross "%%f" -o "..\compiled\dxil\%%~nf.dxil"
    )
)

for %%f in (*.frag.hlsl) do (
    if exist "%%f" (
        shadercross "%%f" -o "..\compiled\spirv\%%~nf.spv"
        shadercross "%%f" -o "..\compiled\msl\%%~nf.msl"
        shadercross "%%f" -o "..\compiled\dxil\%%~nf.dxil"
    )
)

for %%f in (*.comp.hlsl) do (
    if exist "%%f" (
        shadercross "%%f" -o "..\compiled\spirv\%%~nf.spv"
        shadercross "%%f" -o "..\compiled\msl\%%~nf.msl"
        shadercross "%%f" -o "..\compiled\dxil\%%~nf.dxil"
    )
)