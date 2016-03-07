@echo off

for /r %%F in ("*.vert") do (
  echo Building vertex shader '%%~nxF'
  glslangValidator.exe -V %%~nxF | findstr "ERROR"
  move /Y simple.spv ..\resources\%%~nxF.spv >nul 2>&1
  echo.
)

for /r %%F in ("*.frag") do (
  echo Building fragment shader '%%~nxF'
  glslangValidator.exe -V %%~nxF | findstr "ERROR"
  move simple.spv ..\resources\%%~nxF.spv >nul 2>&1
  echo.
)


@pause