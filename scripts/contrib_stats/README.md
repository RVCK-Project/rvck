贡献统计工具集介绍
----

<!-- TOC -->

- [1. 概述](#1-概述)
- [2. 贡献统计工具](#2-贡献统计工具)
	- [2.1. 功能特性](#21-功能特性)
	- [2.2. 使用说明](#22-使用说明)
		- [2.2.1. 查看统计报告](#221-查看统计报告)
		- [2.2.2. 调试方法](#222-调试方法)
		- [2.2.3. 自定义配置](#223-自定义配置)
- [3. 处理丢弃补丁工具](#3-处理丢弃补丁工具)
- [4. 许可证](#4-许可证)

<!-- /TOC -->

# 1. 概述

本文用于介绍 `contrib-stats` 分支下用于支持贡献统计的工具，目前主要包括：

- 贡献统计工具
- 处理丢弃补丁工具

文件目录结构：

```
contrib-stats/
├── .github/workflows/update-contrib-stats.yml    # GitHub Actions工作流
├── scripts/contrib_stats/
│   ├── add-dropped-patchinfo.sh                  # 用于添加被丢弃补丁信息的脚本
│   ├── company_stats.py                          # 贡献统计脚本
│   ├── dropped-patches.db                        # 被丢弃补丁信息数据库
│   ├── README.md                                 # 本文件
│   └── requirements.txt                          # Python 依赖
├── docs/                                         # 贡献统计报告
│   ├── index.md                                  # 统计主页
│   ├── companies/                                # 各机构详情
│   │   ├── 超睿科技.md
│   │   ├── 进迭时空.md
│   │   └── ...
│   └── data/                                     # 原始数据
│       ├── latest.json                           # 最新数据
│       └── stats_*.json                          # 历史数据
└── README.md                                     # contrib-stats 分支说明文件
```

# 2. 贡献统计工具

`company_stats.py` 用于对主开发分支的贡献进行统计，该工具会从主分支获取数据进行分析，生成详细的贡献统计报告并更新 `docs` 目录下的报告文件。

## 2.1. 功能特性

- 生成可视化统计报告和各机构详情
- 支持 **手动触发/自动触发** 两种方式运行该统计脚本
  - **手动触发** 是指通过 GitHub Actions 页面手动运行，但由于 `contrib-stats` 分支不是 RVCK 仓库 <https://github.com/RVCK-Project/rvck> 的默认分支，所以无法在 RVCK 仓库中手动触发。但可以通过远程 API 方式触发。
  - **自动触发** 是指当 `contrib-stats` 分支有更新时会触发贡献统计脚本执行。**(TODO)**: 暂不支持检测主分支更新及时触发。

## 2.2. 使用说明

### 2.2.1. 查看统计报告

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

### 2.2.2. 调试方法

- 查看 GitHub Actions 运行日志

- 本地运行脚本测试

  示例：`python scripts/contrib_stats/company_stats.py --remote-url git@github.com:RVCK-Project/rvck.git`

  可以打开调试开关，运行期间会输出更多内容

  示例：`DEBUG=1 python scripts/contrib_stats/company_stats.py --remote-url git@github.com:RVCK-Project/rvck.git`

- 检查生成的 JSON 数据文件

### 2.2.3. 自定义配置

- 修改机构配置

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

- 修改统计参数

  - 主分支: 修改 `--main-branch` 参数或设置 `MAIN_BRANCH` secret
  - 克隆深度: 修改 `self.clone_depth` 变量
  - 克隆深度内未找到 tag 时的默认统计数量: 修改 `self.fallback_count` 变量

# 3. 处理丢弃补丁工具

考虑到 RVCK 仓库定期会升级上游的基线（譬如从 `v6.6.158` 升级到 `v6.6.161`），由于上游升级中 backport 的补丁可能会和 RVCK 自己 backport 的补丁重复，所以一旦发生补丁重复，默认的做法是会丢弃 RVCK 自己 backport 的补丁。但这样可能会导致丢失贡献统计值。所以我们需要在统计时补上丢弃的补丁的贡献信息。

为此我们会维护一个用于保存历史上丢弃的 RVCK backport 的补丁贡献信息的数据库，即 `dropped-patches.db`。运行贡献统计脚本时会查看这个数据库，将历史上丢弃的补丁的贡献信息重新补上。

`dropped-patches.db` 文件为纯文本文件，由多个类似下面的项组成，每一项对应一个在升级基线过程中因为和上游重复而丢弃的 backport 的补丁，但只保存了我们感兴趣的贡献相关信息。具体项的例子如下所示。

```
<begin>
Commit:3ab72afb807107ee1f4188f2656da4dad4d5ff2d
Author:Mayuresh Chitale <mchitale@ventanamicro.com>
Subject:RISC-V: KVM: Add kvm_vcpu_config
Body:
mainline inclusion
from Linux 6.6-rc6
commit fe0bab701e3b71a1d2db123c05b4f5c33098cc96
category: feature
bugzilla: https://github.com/RVCK-Project/rvck/issues/67

--------------------------------

Add a placeholder for all registers such as henvcfg, hstateen etc
which have 'static' configurations depending on extensions supported by
the guest. The values are derived once and are then subsequently written
to the corresponding CSRs while switching to the vcpu.

Signed-off-by: Mayuresh Chitale <mchitale@ventanamicro.com>
Signed-off-by: Anup Patel <anup@brainfault.org>
Signed-off-by: yechao-w <wang.yechao255@zte.com.cn>
Signed-off-by: Yanteng Si <si.yanteng@linux.dev>
Signed-off-by: Yanteng Si <siyanteng@iscas.ac.cn>
<end>
```

每次我们升级 RVCK 基线时需要收集可能存在的被丢弃的重复补丁的 hashcode，组织成一个文本文件，譬如叫 `patches.txt`, 然后运行 `add-dropped-patchinfo.sh`, 读取这个 `patches.txt`，将这些丢弃的补丁信息添加到我们的数据库文件 `dropped-patches.db` 中，这样我们在运行贡献统计脚本时就会自动补上丢弃的贡献信息了。具体运行 `add-dropped-patchinfo.sh` 的方法如下：

```bash
$ ./add-dropped-patchinfo.sh <git-repo-path> <input-hash-file> <output-file>
```

其中：

- `<git-repo-path>` 是 RVCK 仓库的路径，前提是需要确保本地的 RVCK 仓库已经 fetch 了最新的远程 tag，假设本次基线升级是从 `v6.6.158` 升级到 `v6.6.161`, 则本地必须要有 `v6.6.158-rvck` 这个 tag（可以运行 `git tag |  grep rvck` 进行查看）。

- `<input-hash-file>` 是保存了本次升级丢弃的补丁的 hashcode 的文本文件的路径，文本文件的例子如下：

  ```txt
  3ab72afb8071
  2e2b021be174
  96d2105514f9
  36fdd483cc59
  f81895929b0f
  0cc4dc08dc60
  ```

  这个文件不需要维护在本仓库中，因为每次升级时才可能产生这个文件（需要手动编辑），也可能没有这个文件（即本次升级没有丢弃的补丁）。至于每次升级添加了哪些被丢弃的补丁，可以通过查看 `contrib-stats` 分支的提交历史来获取，因为我们会维护 `dropped-patches.db` 文件，查看这个文件的修改历史就可以直到我们的添加历史。

- `<output-file>`: 即 `dropped-patches.db` 的路径。 

具体操作时，可以在 RVCK 目录下（`contrib-stats` 分支下）执行：

```shell
$ ./scripts/contrib_stats/add-dropped-patchinfo.sh . <path-to-hash-file> ./scripts/contrib_stats/dropped-patches.db
```

# 4. 许可证

统计脚本和报告遵循与主项目相同的许可证。
