# 删除旧的 build 目录（清理旧文件）
if (Test-Path build) { Remove-Item -Recurse -Path build -Force }

# 创建目录
mkdir build | Out-Null

# 进入目录
Set-Location build

# 1. 生成 Makefile (Release 模式)
# -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 是可选的，有助于 VS Code 的 C++ 插件智能感知
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# 2. 并行编译
# 获取 CPU 逻辑核心数
$cpuCount = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
Write-Host "Detected $cpuCount CPU cores. Starting parallel build..." -ForegroundColor Cyan

# 使用 cmake --build 抽象命令，-j 指定并行数
cmake --build . --config Release -j $cpuCount

# 返回上级
Set-Location ..