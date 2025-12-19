#!/bin/bash

# 1. 清理旧构建 (对应 PowerShell 的 Remove-Item)
if [ -d "build" ]; then
    echo "Cleaning old build directory..."
    rm -rf build
fi

# 2. 创建并进入目录
mkdir build
cd build

# 3. 生成 Makefile (Linux 模式)
# 注意：移除了 "MinGW Makefiles"，Linux 下默认使用 Unix Makefiles
echo "Generating Makefiles..."
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# 4. 并行编译
# nproc 命令自动获取当前机器的核心数 (对应 PowerShell 的 Get-CimInstance)
CPU_COUNT=$(nproc)
echo "Detected $CPU_COUNT CPU cores. Starting parallel build..."

# 使用 cmake --build 进行编译
cmake --build . --config Release -j $CPU_COUNT

echo "Build finished."