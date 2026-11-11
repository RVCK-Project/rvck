# 贡献统计

本仓库的 `contrib-stats` 分支专门用于存储 RVCK 贡献统计数据，独立于 RVCK 主开发分支 (`rvck-6.6`)。

## 📁 目录结构

```
contrib-stats/
├── .github/workflows/          # GitHub Actions 工作流
├── scripts/contrib_stats/      # 统计脚本
├── docs/                       # 统计报告
│   ├── index.md               # 统计主页
│   ├── companies/             # 各机构详情
│   └── data/                  # 原始数据
└── README.md                  # 本文件
```

## 🔗 查看报告

- 📊 [完整统计报告](docs/index.md)
- 📁 [原始数据](docs/data/latest.json)

## ⚙️ 使用说明

### 触发

- 当统计脚本更新时自动触发
- 主分支有变更时需手动触发

### 本地查看

```bash
# 切换到统计分支
git checkout contrib-stats

# 查看统计报告
# 用浏览器打开 docs/index.md
```

## 📄 许可证

统计脚本和报告遵循与主项目相同的许可证。
