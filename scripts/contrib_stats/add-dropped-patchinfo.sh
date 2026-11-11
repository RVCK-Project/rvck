#!/usr/bin/env bash
#
# add-dropped-patchinfo.sh
#
# 用法: ./add-dropped-patchinfo.sh <git-repo-path> <input-hash-file> <output-file>
#
# 从输入文件中读取每一行 hashcode，在指定的 git 仓库中查找对应补丁的
# Author / Subject / Body 信息，并按指定格式追加到输出文件中。
#
set -euo pipefail

usage() {
    echo "Usage: $0 <git-repo-path> <input-hash-file> <output-file>" >&2
    exit 1
}

# 检查参数个数
if [ "$#" -ne 3 ]; then
    usage
fi

REPO_DIR="$1"
INPUT_FILE="$2"
OUTPUT_FILE="$3"

# 检查 git 仓库目录是否存在
if [ ! -d "$REPO_DIR" ]; then
    echo "Error: git repo directory '$REPO_DIR' not found." >&2
    exit 1
fi

# 将仓库路径转为绝对路径，便于后续在子 shell 中 cd
REPO_DIR="$(cd "$REPO_DIR" && pwd)"

# 确认指定目录是一个 git 仓库（工作区或裸仓库均可）
if ! git -C "$REPO_DIR" rev-parse --git-dir >/dev/null 2>&1; then
    echo "Error: '$REPO_DIR' is not a git repository." >&2
    exit 1
fi

# 检查输入文件是否存在且可读
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: input file '$INPUT_FILE' not found." >&2
    exit 1
fi
if [ ! -r "$INPUT_FILE" ]; then
    echo "Error: input file '$INPUT_FILE' is not readable." >&2
    exit 1
fi

# 逐行读取 hash
while IFS= read -r raw_line || [ -n "$raw_line" ]; do
    # 去掉首尾空白（含 \r，防止 Windows 换行）
    hash="$(printf '%s' "$raw_line" | tr -d '\r' | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"

    # 跳过空行与注释行
    [ -z "$hash" ] && continue
    case "$hash" in
        \#*) continue ;;
    esac

    # 校验该 commit 是否存在
    if ! full_hash="$(git -C "$REPO_DIR" rev-parse --verify "${hash}^{commit}" 2>/dev/null)"; then
        echo "Warning: commit '$hash' not found in repository '$REPO_DIR', skipped." >&2
        continue
    fi

    # 获取补丁信息
    author="$(git -C "$REPO_DIR" log -1 --format='%an <%ae>' "$full_hash")"
    subject="$(git -C "$REPO_DIR" log -1 --format='%s' "$full_hash")"
    body="$(git -C "$REPO_DIR" log -1 --format='%b' "$full_hash")"

    # 追加写入输出文件
    {
        echo "<begin>"
        echo "Commit:${full_hash}"
        echo "Author:${author}"
        echo "Subject:${subject}"
        echo "Body:"
        if [ -n "$body" ]; then
            printf '%s\n' "$body"
        fi
        echo "<end>"
    } >> "$OUTPUT_FILE"

    echo "Added patch info for $full_hash" >&2
done < "$INPUT_FILE"

echo "Done. Output written to '$OUTPUT_FILE'." >&2