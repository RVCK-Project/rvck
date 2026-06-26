# 贡献统计工具

## 概述

该工具用于主开发分支的贡献统计，从主分支获取数据进行分析，生成详细的贡献统计报告。

## 功能特性

- ✅ **详细报告**: 生成可视化统计报告和各机构详情
- ✅ **手动/自动触发**: 可通过GitHub Actions页面手动运行，每日定时自动更新
- ✅ **脚本更新触发**: 统计脚本更新时自动运行
-    **(TODO)**: 暂不支持检测主分支更新及时触发
## 使用说明

### 查看统计报告

1. **切换到统计分支**:
   ```bash
   git checkout contrib-stats
   ```

2. **查看统计报告**:
   - 主报告: `docs/index.md`
   - 各机构详情: `docs/companies/`
   - 原始数据: `docs/data/`

3. **在线查看**:
   - 在GitHub上切换到 `contrib-stats` 分支
   - 浏览 `docs/` 目录下的文件

### 更新统计数据

#### 手动更新
1. 进入仓库的 **Actions** 页面
2. 选择 **Update Contribution Stats** 工作流
3. 点击 **Run workflow**

#### 自动触发
- 当统计脚本更新时自动触发
- 每日定时自动触发

## 统计规则

### RVCK 贡献机构识别规则
- 超睿科技: @ultrarisc.com
- 进迭时空: @spacemit.com, @linux.spacemit.com
- 中兴通讯: @zte.com.cn
- 阿里: @linux.alibaba.com
- 算能: @sophgo.com
- 软件所: @iscas.ac.cn, @isrc.iscas.ac.cn, Weihao Li <ieiao@outlook.com>
- 蓝芯算力: @lanxincomputing.com

### 提交归属统计规则

贡献统计目前只面向对 RVCK 仓库有贡献的参与机构，因此在对主线补丁反合至 RVCK 等工作中，原补丁作者所属机构暂不计入该统计。

在 RVCK 贡献机构范围中，提交归属统计基于以下规则：

1. **优先原则**: 如果提交的 Author 邮箱属于某机构，则该提交只计入该机构
2. **签名统计**: 如果 Author 不属于任何机构，则统计签名中出现的所有机构
3. **去重规则**: 每个提交对每个机构最多计1次

## 文件结构

```
contrib-stats/
├── .github/workflows/update-contrib-stats.yml    # GitHub Actions工作流
├── scripts/contrib_stats/                        # 统计脚本
│   ├── company_stats.py                          # 主统计脚本
│   ├── requirements.txt                          # Python依赖
│   └── README.md                                 # 本文件
├── docs/                                         # 统计报告
│   ├── index.md                                  # 统计主页
│   ├── companies/                                # 各机构详情
│   │   ├── 超睿科技.md
│   │   ├── 进迭时空.md
│   │   └── ...
│   └── data/                                     # 原始数据
│       ├── latest.json                           # 最新数据
│       └── stats_*.json                          # 历史数据
└── README.md                                     # 分支说明
```

## 自定义配置

### 修改机构配置
编辑 `scripts/contrib_stats/company_stats.py` 中的 `companies` 字典:
```python
self.companies = {
    "机构名": {
        "suffixes": ["@邮箱后缀"],        # 邮箱后缀匹配
        "specific_emails": ["特定签名"],  # 特定邮箱匹配
        "color": "#颜色代码"              # 页面显示颜色
    },
    # ...
}
```

### 修改统计参数
- 主分支: 修改 `--main-branch` 参数或设置 `MAIN_BRANCH` secret
- 克隆深度: 修改 `self.clone_depth` 变量
- 克隆深度内未找到 tag 时的默认统计数量: 修改 `self.fallback_count` 变量

## 故障排除

### 常见问题

1. **报告未更新**
   - 检查GitHub Actions运行状态
   - 检查是否有文件变更，没有变更将不会更新

2. **数据不准确**
   - 检查机构邮箱配置
   - 检查统计规则是否符合预期

### 调试方法
- 查看GitHub Actions运行日志
- 本地运行脚本测试
- 检查生成的JSON数据文件

## 许可证

统计脚本和报告遵循与主项目相同的许可证。
