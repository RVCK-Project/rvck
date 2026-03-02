#!/usr/bin/env python3
"""
贡献统计脚本
运行在 contrib-stats 分支，从主开发分支 rvck-6.6 获取数据进行分析
默认数据统计范围是最新提交至向后找到的第一个 tag 节点
"""

import subprocess
import re
import os
import sys
import json
import argparse
import tempfile
import urllib.parse
import shutil
from datetime import datetime, timedelta
from collections import defaultdict
from pathlib import Path

def beijing_timestamp():
    """返回北京时间字符串"""

    # 获取当前UTC时间
    utc_now = datetime.utcnow()
    # 转换为北京时间
    bj_time = utc_now + timedelta(hours=8)
    return bj_time.strftime("%Y-%m-%d %H:%M:%S")

class ContribStats:
    def __init__(self, main_branch="rvck-6.6", stats_branch="contrib-stats", remote_url=None):
        self.main_branch = main_branch
        self.stats_branch = stats_branch
        self.remote_url = remote_url or f"https://github.com/{os.environ.get('GITHUB_REPOSITORY', '')}.git"
        self.repo_path = Path(".").absolute()

        # 配置参数
        self.clone_depth = 3000  # 默认克隆深度，rvck 贡献量超过该数值时需要更新
        self.fallback_count = 1000  # 未找到 tag 时的默认统计数量

        # 机构配置
        self.companies = {
            "超睿科技": {
                "suffixes": ["@ultrarisc.com"],
                "specific_emails": [],
                "color": "#4CAF50"
            },
            "进迭时空": {
                "suffixes": ["@spacemit.com", "@linux.spacemit.com"],
                "specific_emails": [],
                "color": "#2196F3"
            },
            "中兴通讯": {
                "suffixes": ["@zte.com.cn"],
                "specific_emails": [],
                "color": "#FF9800"
            },
            "阿里达摩院": {
                "suffixes": ["@linux.alibaba.com"],
                "specific_emails": [],
                "color": "#F44336"
            },
            "算能": {
                "suffixes": ["@sophgo.com"],
                "specific_emails": [],
                "color": "#9C27B0"
            },
            "软件所": {
                "suffixes": ["@iscas.ac.cn", "@isrc.iscas.ac.cn"],
                "specific_emails": ["Weihao Li <ieiao@outlook.com>"],
                "color": "#FF9800"
            }
        }

        # 输出目录
        self.docs_dir = self.repo_path / "docs"
        self.companies_dir = self.docs_dir / "companies"
        self.data_dir = self.docs_dir / "data"

        # 创建目录
        self.docs_dir.mkdir(parents=True, exist_ok=True)
        self.companies_dir.mkdir(parents=True, exist_ok=True)
        self.data_dir.mkdir(parents=True, exist_ok=True)

        # 时间戳
        self.timestamp = beijing_timestamp()

        # 主分支信息
        self.main_commit = self.get_main_branch_commit()

    def run_git(self, cmd, cwd=None, check_error=True):
        """运行git命令"""
        if cwd is None:
            cwd = self.repo_path

        result = subprocess.run(
            cmd,
            shell=True,
            cwd=cwd,
            capture_output=True,
            text=True,
            encoding='utf-8'
        )

        if check_error and result.returncode != 0:
            print(f"Git命令失败: {cmd}")
            print(f"错误输出: {result.stderr}")

        return result.stdout.strip(), result.stderr.strip(), result.returncode

    def get_main_branch_commit(self):
        """获取主分支的最新提交哈希"""

        # 获取主开发分支的最新提交
        stdout, stderr, code = self.run_git(f"git ls-remote {self.remote_url} {self.main_branch}")
        if code == 0 and stdout:
            parts = stdout.split()
            if len(parts) >= 1:
                return parts[0][:8]

        return "unknown"

    def clone_shallow_repo(self, tmp_dir):
        """创建浅克隆仓库，返回凭证文件路径以便后续清理"""

        print(f"创建浅克隆（深度: {self.clone_depth}）...")

        # 1. 访问原始 URL（可能适用于公共仓库）
        clone_cmd = (
            f"git clone --bare --filter=blob:none "
            f"--depth={self.clone_depth} "
            f"{self.remote_url} "
            f"{tmp_dir}"
        )

        stdout, stderr, code = self.run_git(clone_cmd, check_error=False)

        if code == 0:
            print(f"✓ 浅克隆创建完成（原始 URL）")
            return True, None  # 成功，没有凭证文件

        # 2. 原始 URL 失败，尝试使用GitHub Token
        github_token = os.environ.get('GITHUB_TOKEN', '')
        if not github_token:
            print(f"❌ 原始URL克隆失败且无GITHUB_TOKEN: {stderr[:100]}")
            return False, None

        print("原始 URL 克隆失败，尝试使用 GitHub Token...")

        # 清理目标目录
        if os.path.exists(tmp_dir):
            print(f"清理目录以进行Token认证尝试: {tmp_dir}")
            shutil.rmtree(tmp_dir, ignore_errors=True)

        # 创建临时凭据文件
        parsed_url = urllib.parse.urlparse(self.remote_url)
        cred_content = f"https://x-access-token:{github_token}@{parsed_url.netloc}"
        cred_file = f"{tmp_dir}.git-credentials"

        try:
            with open(cred_file, 'w') as f:
                f.write(cred_content)

            # 配置git使用凭据文件
            self.run_git(f"git config --global credential.helper 'store --file={cred_file}'", check_error=False)

            # 再次尝试克隆
            stdout, stderr, code = self.run_git(clone_cmd, check_error=False)

            if code == 0:
                print(f"✓ 浅克隆创建完成（使用Token）")
                return True, cred_file
            else:
                print(f"❌ Token认证也失败: {stderr[:200]}")
                return False, cred_file

        except Exception as e:
            print(f"❌ 设置凭据失败: {e}")
            return False, None

    def cleanup_credentials(self, cred_file):
        """清理凭证文件"""

        try:
            if cred_file and os.path.exists(cred_file):
                os.remove(cred_file)
            self.run_git("git config --global --unset credential.helper", check_error=False)
            print("✓ 凭证已清理")
        except Exception as e:
            print(f"⚠️ 清理凭据时出错: {e}")

    def find_tag_in_clone(self, tmp_dir):
        """在克隆的仓库中查找tag"""

        print("在克隆中查找tag...")

        # 获取提交日志，查找tag
        log_cmd = f"git log --oneline --decorate -n {self.clone_depth}"
        stdout, stderr, code = self.run_git(log_cmd, cwd=tmp_dir)

        if code != 0:
            print(f"获取日志失败: {stderr}")
            return None

        # 查找包含tag的行
        for line in stdout.split('\n'):
            if 'tag:' in line:
                # 提取tag名称
                # 示例: "abc1234 (tag: v6.6.112) commit message"
                tag_match = re.search(r'tag:\s*([^,\s)]+)', line)
                if tag_match:
                    tag = tag_match.group(1)
                    print(f"找到tag: {tag}")
                    return tag

        print(f"在最近{self.clone_depth}个提交中未找到tag")
        return None

    def get_commits_from_clone(self, tmp_dir, tag):
        """从克隆中获取提交列表"""

        # 确定统计范围
        if tag:
            range_spec = f"{tag}..HEAD"
            log_cmd = f"git log {range_spec} --no-merges --format='%H|%an|%ae|%ad|%s' --date=iso"
            print(f"统计范围: {tag}..HEAD")
        else:
            range_spec = f"-{self.fallback_count}"
            log_cmd = f"git log {range_spec} --no-merges --format='%H|%an|%ae|%ad|%s' --date=iso"
            print(f"统计范围: 最近{self.fallback_count}个提交")

        stdout, stderr, code = self.run_git(log_cmd, cwd=tmp_dir)

        if code != 0:
            print(f"获取提交失败: {stderr}")
            return None

        # 解析提交数据
        commits = []
        for line in stdout.split('\n'):
            if '|' not in line:
                continue
            parts = line.split('|', 4)
            if len(parts) == 5:
                commits.append({
                    'hash': parts[0],
                    'author_name': parts[1],
                    'author_email': parts[2],
                    'date': parts[3],
                    'subject': parts[4]
                })

        print(f"获取到 {len(commits)} 个提交")
        return commits

    def get_commit_signatures(self, tmp_dir, commit_hash):
        """从克隆中获取提交的签名信息"""

        show_cmd = f"git show --no-patch --format=%B {commit_hash}"
        stdout, stderr, code = self.run_git(show_cmd, cwd=tmp_dir)

        if code != 0:
            return []

        signatures = []
        for line in stdout.split('\n'):
            if line.strip().lower().startswith('signed-off-by:'):
                signatures.append(line.strip())

        return signatures

    def get_company_by_email(self, email):
        """根据邮箱判断机构归属"""

        if not email:
            return None

        email_lower = email.lower()

        for company, info in self.companies.items():
            # 检查邮箱后缀
            for suffix in info["suffixes"]:
                if suffix.lower() in email_lower:
                    return company

            # 检查特定邮箱
            for specific_email in info["specific_emails"]:
                email_match = re.search(r'<([^>]+)>', specific_email)
                if email_match:
                    specific_email_addr = email_match.group(1).lower()
                    if specific_email_addr in email_lower:
                        return company

        return None

    def analyze_commits(self):
        """分析提交数据"""

        print("开始分析提交...")

        with tempfile.TemporaryDirectory() as tmp_dir:
            cred_file = None
            try:
                # 1. 创建浅克隆
                clone_success, cred_file = self.clone_shallow_repo(tmp_dir)
                if not clone_success:
                    return None

                # 2. 查找tag
                start_tag = self.find_tag_in_clone(tmp_dir)

                # 3. 获取提交列表
                commits = self.get_commits_from_clone(tmp_dir, start_tag)
                if not commits:
                    return None

                # 4. 初始化统计数据结构
                stats = {
                    'total_commits': len(commits),
                    'commits_with_company': 0,
                    'companies': {company: {'count': 0, 'commits': []} for company in self.companies},
                    'no_company_commits': [],
                    'generated_at': self.timestamp,
                    'main_branch': self.main_branch,
                    'main_commit': self.main_commit,
                    'start_tag': start_tag or f'最近{self.fallback_count}个提交'
                }

                # 5. 分析每个提交
                for i, commit in enumerate(commits):
                    if (i + 1) % 50 == 0:
                        print(f"已分析 {i + 1}/{len(commits)} 个提交")

                    # 获取签名信息
                    signatures = self.get_commit_signatures(tmp_dir, commit['hash'])
                    commit['signatures'] = signatures

                    # 确定提交所属机构
                    author_company = self.get_company_by_email(commit['author_email'])

                    if author_company:
                        # Author属于某个机构，只统计该机构
                        stats['companies'][author_company]['count'] += 1
                        stats['companies'][author_company]['commits'].append(commit)
                        stats['commits_with_company'] += 1
                    else:
                        # Author不属于任何机构，检查签名中的机构
                        signature_companies = set()
                        for sig in signatures:
                            # 从签名中提取邮箱
                            email_match = re.search(r'<([^>]+)>', sig)
                            if email_match:
                                email = email_match.group(1)
                                company = self.get_company_by_email(email)
                                if company:
                                    signature_companies.add(company)

                        if signature_companies:
                            # 统计所有出现的机构
                            stats['commits_with_company'] += 1
                            for company in signature_companies:
                                stats['companies'][company]['count'] += 1
                                stats['companies'][company]['commits'].append(commit)
                        else:
                            # 没有机构相关签名
                            stats['no_company_commits'].append(commit)

                return stats

            finally:
                # 确保凭证被清理
                self.cleanup_credentials(cred_file)

    def generate_main_page(self, stats):
        """生成统计主页"""
        # 按贡献数排序
        sorted_companies = sorted(
            [(company, stats['companies'][company]['count']) for company in self.companies],
            key=lambda x: x[1],
            reverse=True
        )

        # 生成Markdown内容
        content = f"""# 📊 内核贡献统计报告

**最后更新时间**: {stats['generated_at']}
**主分支**: `{stats['main_branch']}`
**主分支提交**: {stats['main_commit']}
**统计起始点**: `{stats['start_tag']}`

---

## 总体统计

| 项目 | 数量 |
|------|------|
| 总提交数 | {stats['total_commits']} |
| 有机构贡献的提交 | {stats['commits_with_company']} |
| 无机构贡献的提交 | {len(stats['no_company_commits'])} |

## 各机构贡献统计

| 机构 | 提交数 | 占比 | 图表 |
|------|--------|------|------|
"""

        for company, count in sorted_companies:
            if stats['total_commits'] > 0:
                percentage = (count / stats['total_commits'] * 100)
                # 生成简单的进度条
                bar_length = int(percentage / 2)  # 50个字符对应100%
                bar = "█" * bar_length + "░" * (50 - bar_length)
                content += f"| [{company}](companies/{company}.md) | {count} | {percentage:.1f}% | `{bar}` |\n"
            else:
                content += f"| [{company}](companies/{company}.md) | {count} | 0.0% | `░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░` |\n"

        content += f"""
## 📈 可视化图表

```mermaid
pie title 各机构贡献占比
"""

        # 添加Mermaid饼图数据
        for company, count in sorted_companies:
            if count > 0 and stats['total_commits'] > 0:
                # 计算百分比
                percentage = (count / stats['total_commits'] * 100)
                content += f'    "{company} ({count}, {percentage:.1f}%)" : {count}\n'

        # 添加无机构贡献的部分
        if len(stats['no_company_commits']) > 0 and stats['total_commits'] > 0:
            no_company_percentage = (len(stats['no_company_commits']) / stats['total_commits'] * 100)
            content += f'    "其他 ({len(stats["no_company_commits"])}, {no_company_percentage:.1f}%)" : {len(stats["no_company_commits"])}\n'

        content += """```

## 📋 统计规则说明

### 1. 机构识别规则
"""

        for company, info in self.companies.items():
            content += f"- **{company}**: {', '.join(info['suffixes'])}"
            if info['specific_emails']:
                content += f"，特定签名：{', '.join(info['specific_emails'])}"
            content += "\n"

        content += f"""
### 2. 提交归属规则

贡献统计目前只面向对 RVCK 仓库有贡献的参与机构，因此在对主线补丁反合至 RVCK
等工作中，原补丁作者所属机构暂不计入该统计。

在 RVCK 贡献机构范围中，提交归属统计基于以下规则：

1. **优先原则**: 如果提交的 Author 邮箱属于某机构，则该提交只计入该机构
2. **签名统计**: 如果 Author 不属于任何机构，则统计签名中出现的所有机构
3. **去重规则**: 每个提交对每个机构最多计1次

### 3. 统计范围
- 主分支: `{stats['main_branch']}`
- 起始点: {stats['start_tag']}
- 结束点: 统计时的最新提交

---

## 🔧 技术说明

- **统计分支**: `contrib-stats`
- **统计脚本**: [scripts/contrib_stats/](scripts/contrib_stats/)
- **更新方式**: 手动/定时触发，脚本更新时自动触发
- **原始数据**: [data/latest.json](data/latest.json)

---

*最后更新: {stats['generated_at']}*
*生成自主分支提交: {stats['main_commit']}*
"""

        return content

    def generate_company_page(self, company, info, stats):
        """生成单个机构的详情页"""

        company_stats = stats['companies'][company]

        content = f"""# {company} 贡献详情

<div style="background-color: {info['color']}20; padding: 15px; border-radius: 8px; border-left: 5px solid {info['color']};">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: {company_stats['count']}</li>
<li><strong>统计时间</strong>: {stats['generated_at']}</li>
<li><strong>主分支</strong>: {stats['main_branch']}</li>
<li><strong>起始标签</strong>: {stats['start_tag']}</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: {', '.join(info['suffixes'])}
"""

        if info['specific_emails']:
            content += f"- **特定签名**: {', '.join(info['specific_emails'])}\n"

        content += f"""
## 📋 提交列表

| 提交哈希 | 日期 | 作者 | 标题 |
|----------|------|------|------|
"""

        # 显示所有提交（如果没有太多）
        max_display = 200  # 最多显示200个
        display_commits = company_stats['commits'][:max_display]

        for commit in display_commits:
            date_short = commit['date'][:10] if 'T' in commit['date'] else commit['date'][:10]
            repo = os.environ.get('GITHUB_REPOSITORY', 'your/repo')
            # 链接指向具体的 commit
            content += f"| [{commit['hash'][:8]}](https://github.com/{repo}/commit/{commit['hash']}) | {date_short} | {commit['author_name']} | {commit['subject'][:80]}... |\n"

        if company_stats['count'] > max_display:
            content += f"\n*注：只显示前{max_display}个提交，共 {company_stats['count']} 个提交*\n"

        content += f"""

## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 {stats['generated_at']}*
*数据来源: 主分支 {stats['main_branch']}@{stats['main_commit']}*
"""

        return content

    def save_statistics(self, stats):
        """保存所有统计数据"""

        print("正在保存统计数据...")
        # 1. 保存原始数据
        latest_file = self.data_dir / "latest.json"
        timestamp_file = self.data_dir / f"stats_{beijing_timestamp().replace('-', '').replace(' ', '_').replace(':', '')}.json"

        with open(latest_file, 'w', encoding='utf-8') as f:
            json.dump(stats, f, ensure_ascii=False, indent=2)

        with open(timestamp_file, 'w', encoding='utf-8') as f:
            json.dump(stats, f, ensure_ascii=False, indent=2)

        print(f"✓ 原始数据已保存: {latest_file}")

        # 2. 生成并保存主页面
        main_page = self.generate_main_page(stats)
        main_page_file = self.docs_dir / "index.md"

        with open(main_page_file, 'w', encoding='utf-8') as f:
            f.write(main_page)

        print(f"✓ 统计主页已生成: {main_page_file}")

        # 3. 生成并保存各机构页面
        for company, info in self.companies.items():
            if stats['companies'][company]['count'] > 0:
                company_page = self.generate_company_page(company, info, stats)
                company_file = self.companies_dir / f"{company}.md"

                with open(company_file, 'w', encoding='utf-8') as f:
                    f.write(company_page)

                print(f"✓ 机构页面已生成: {company_file}")

        # 4. 生成分支README
        branch_readme = self.generate_branch_readme(stats)
        with open(self.repo_path / "README.md", 'w', encoding='utf-8') as f:
            f.write(branch_readme)

        print("✓ 分支README已更新")

        return True

    def generate_branch_readme(self, stats):
        """生成统计分支的README"""

        # 按贡献数排序
        sorted_companies = sorted(
            [(company, stats['companies'][company]['count']) for company in self.companies],
            key=lambda x: x[1],
            reverse=True
        )

        rank_content = ""
        for i, (company, count) in enumerate(sorted_companies, 1):
            if count > 0:
                rank_content += f"{i}. **{company}**: {count} 个提交\n"

        return f"""# 贡献统计分支

该分支专门用于存储内核贡献统计数据，独立于 RVCK 主开发分支(rvck-6.6)。

## 📊 最新统计摘要

**统计时间**: {stats['generated_at']}
**主分支**: `{stats['main_branch']}`
**统计起始点**: `{stats['start_tag']}`

| 项目 | 数量 |
|------|------|
| 总提交数 | {stats['total_commits']} |
| 有机构贡献的提交 | {stats['commits_with_company']} |

**贡献排名**:
{rank_content}
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

### 手动更新统计
1. 进入 GitHub Actions 页面
2. 选择 "Update Contribution Stats"
3. 点击 "Run workflow"

### 自动触发
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

---

*本分支最后更新于 {stats['generated_at']}*
"""

    def run(self):
        """运行完整统计流程"""

        print("=" * 60)
        print("贡献统计系统")
        print("=" * 60)
        print(f"主分支: {self.main_branch}")
        print(f"统计分支: {self.stats_branch}")
        print(f"远程仓库: {self.remote_url}")
        print(f"克隆深度: {self.clone_depth}")
        print()

        # 分析提交数据
        stats = self.analyze_commits()
        if not stats:
            return False

        print(f"\n统计完成:")
        print(f"- 总提交数: {stats['total_commits']}")
        print(f"- 有机构贡献的提交: {stats['commits_with_company']}")
        print(f"- 无机构贡献的提交: {len(stats['no_company_commits'])}")

        # 输出各机构统计
        print(f"\n各机构贡献:")
        for company in self.companies:
            count = stats['companies'][company]['count']
            if count > 0:
                print(f"  - {company}: {count} 个提交")

        # 保存统计数据
        success = self.save_statistics(stats)

        if success:
            print(f"\n✓ 统计报告已生成:")
            print(f"  - 统计主页: docs/index.md")
            print(f"  - 机构详情: docs/companies/")
            print(f"  - 原始数据: docs/data/")
            print(f"  - 分支说明: README.md")

        return success

def main():
    parser = argparse.ArgumentParser(description='贡献统计')
    parser.add_argument('--main-branch', default='rvck-6.6', help='主开发分支')
    parser.add_argument('--stats-branch', default='contrib-stats', help='统计分支')
    parser.add_argument('--remote-url', help='远程仓库URL')

    args = parser.parse_args()

    print("开始贡献统计...")
    print("注意: 此脚本运行在统计分支，从远程仓库获取数据进行分析")
    print()

    stats = ContribStats(
        main_branch=args.main_branch,
        stats_branch=args.stats_branch,
        remote_url=args.remote_url
    )

    success = stats.run()

    if success:
        print("\n" + "=" * 60)
        print("✓ 统计流程成功完成!")
        print("=" * 60)
        return 0
    else:
        print("\n✗ 统计流程失败!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
