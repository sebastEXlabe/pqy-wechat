# SQLCipher 数据库加密分析

## 概述
WeChat 使用 SQLCipher 加密所有本地 SQLite 数据库。

## 一、SQLCipher PRAGMA 配置项 (14项)

| PRAGMA | 代码位置 | 功能 |
|--------|----------|------|
| cipher_compatibility | 0x188453319 | 兼容模式版本 |
| cipher_default_kdf_iter | 0x18845332e | 默认KDF迭代次数 |
| cipher_default_page_size | 0x188453346 | 默认页大小 |
| cipher_hmac_algorithm | 0x18845335f | HMAC算法选择 |
| cipher_kdf_algorithm | 0x188453375 | KDF算法选择 |
| cipher_default_use_hmac | 0x1884533a7 | 默认HMAC开关 |
| cipher_provider | 0x1884533ce | 加密提供者 |
| cipher_provider_version | 0x1884533de | 提供者版本 |
| cipher_use_hmac | 0x188453402 | HMAC开关 |
| cipher_version | 0x188453412 | 加密版本 |
| cipher_page_size | 0x188453421 | 页大小 |
| cipher_hmac_pgno | 0x188bb046a | HMAC页码(已弃用) |
| cipher_hmac_salt_mask | 0x188bb04c0 | HMAC盐掩码(已弃用) |
| rekey_cipher | 0x188bb03b8 | 重设密钥(已弃用) |

## 二、数据库迁移

```sql
SELECT sqlcipher_export('migrate');
```

## 三、加密信息协议

**Protobuf**: `kernel/gen/protobufs/micromsg/get_db_encrypt_info.pb.cc`

## 四、数据库状态监控日志

```
database with tag[%d], at path[%s], encrypted[%d],
db file info[%llu,%s,%s], wal file info[%llu,%s,%s],
shm file info[%llu,%s,%s], first bak[%llu,%s,%s],
last bak[%llu,%s,%s], used size[%llu], kv info[%s,%s]
```

监控: 主DB/WAL/SHM/备份文件 的加密状态、大小、修改时间

## 五、安全影响
1. 数据库无法直接读取 — SQLCipher 页级加密
2. 密钥仅在内存中 — 需运行时提取
3. KDF 迭代 + HMAC 保护 — 防暴力破解和篡改
