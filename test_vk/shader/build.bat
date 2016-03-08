@echo off

echo -----------------------

for /r %%F in ("*.vert") do (
  echo Building vertex shader '%%~nxF'
  glslangValidator.exe -V %%~nxF -o %%~nF.spv | findstr "ERROR"
  move /Y %%~nF.spv ..\resources\%%~nF.vspv >nul 2>&1
  echo.
)

for /r %%F in ("*.frag") do (
  echo Building vertex shader '%%~nxF'
  glslangValidator.exe -V %%~nxF -o %%~nF.spv | findstr "ERROR"
  move /Y %%~nF.spv ..\resources\%%~nF.fspv >nul 2>&1
  echo.
)

echo ------------------------

@pause
