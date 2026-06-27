# Zoe

**⚠️ 重要提示：本项目已迁移至 [Latch](https://github.com/ProgInsight/Latch)** 🚀

Latch 是 Zoe 的进化版本——一个更强大、功能更丰富且持续维护的 C++ 文件下载库。我们建议所有用户迁移到 Latch 以获取持续更新和改进。

---

一个高性能的 C++ 文件下载库。

[English](README.md) | [简体中文](README_ch.md)

## 特性

- 多协议支持（HTTP/HTTPS、FTP/FTPS）
- 多线程分片下载，支持断点续传
- 支持超大文件（可达 PB 级别）
- 可配置下载速度限制
- 进度监控和实时速度追踪
- 考虑了 HDD 慢速读写和 SSD 有限写入寿命的问题
- 哈希校验（MD5、SHA1、SHA256）
- SSL/TLS 证书验证
- 代理支持
- 跨平台（Windows、Linux、macOS）

## 构建和安装

Zoe 唯一的依赖是 [curl](https://github.com/curl/curl)。

### vcpkg

```bash
vcpkg install zoe
```

### Windows

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
```

## 使用方法

### 基本用法

```cpp
#include <zoe/zoe.h>

int main() {
    Zoe z;
    
    // 配置下载设置
    z.setThreadNum(4);  // 使用 4 个线程
    z.setMaxDownloadSpeed(1024 * 1024);  // 限制速度为 1MB/s
    z.setMinDownloadSpeed(512 * 1024, 5000);  // 要求至少 512KB/s，持续 5 秒
    
    // 开始下载
    auto future = z.start(
        "https://example.com/file.zip",
        "file.zip",
        [](ZoeResult result) {
            if (result == ZoeResult::SUCCESSED) {
                std::cout << "下载成功完成" << std::endl;
            } else {
                std::cout << "下载失败，错误码：" << (int)result << std::endl;
            }
        },
        [](int64_t total, int64_t downloaded) {
            if (total > 0) {
                int progress = (int)((double)downloaded * 100.0 / total);
                std::cout << "进度：" << progress << "%" << std::endl;
            }
        },
        [](int64_t bytes_per_second) {
            std::cout << "当前速度：" << bytes_per_second << " 字节/秒" << std::endl;
        }
    );
    
    // 等待下载完成
    future.wait();
    
    return 0;
}
```

### 命令行工具

Zoe 还提供了命令行工具用于下载文件：

```bash
zoe_tool URL TargetFilePath [ThreadNum] [DiskCacheMb] [MD5] [TmpExpiredSeconds] [MaxSpeed]
```

- URL：下载链接
- TargetFilePath：目标文件保存路径
- ThreadNum：线程数，可选，默认为 `1`
- DiskCacheMb：磁盘缓存大小（MB），默认为 `20MB`
- MD5：目标文件的 MD5 值，可选，如果此值不为空，工具会在下载完成后校验文件 MD5
- TmpExpiredSeconds：临时文件过期时间（秒），可选
- MaxSpeed：最大下载速度（字节/秒）

## 迁移到 Latch

我们已将开发工作转移到 [Latch](https://github.com/ProgInsight/Latch)，它提供了：

- **异步 Turbo I/O**，提升性能
- **更好的错误处理**和恢复机制
- **增强的多线程**能力
- **活跃的开发**和定期更新
- **改进的 API**，更易于集成
- **更全面的测试覆盖**

### 为什么选择 Latch？

Latch 是一个基于 libcurl 构建的 C++ 多线程文件下载库。它将文件分割成多个分片，并行下载，如果中断可以精确地从断点处恢复——并内置了速度控制、哈希校验和异步 Turbo I/O 功能。

## 支持

如果您觉得这个项目有帮助，请考虑通过我的 GitHub 主页支持它。

## 许可证

本项目采用 MIT 许可证 - 详情请参见 [LICENSE](LICENSE) 文件。

---

**🚀 新用户请直接使用 [Latch](https://github.com/ProgInsight/Latch) 以获得最佳体验！**
