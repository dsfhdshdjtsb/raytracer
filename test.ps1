Set-Location .\test
cmake --build build
Set-location .\build
ctest
Set-Location ../..