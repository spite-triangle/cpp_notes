#!/usr/bin/env bash
set -euo pipefail

# ============================================================================
# vfox_install.sh
#
# 将包解压到 vfox 仓库的指定路径下。
#
# 用法:
#   vfox_install.sh [-r <repository>] -t <sdk@version> <package_path>
#
# 选项:
#   -r, --repository    仓库路径
#   -t, --target        安装目标，格式为 'sdk@version'，例如：'java@1.8'
#   -h, --help          显示帮助信息
# ============================================================================

DEFAULT_REPO="$HOME/.vfox/cache"

# ----------------------------------------------------------------------------
# 打印帮助信息
# ----------------------------------------------------------------------------
usage() {
    cat <<EOF
Usage: $(basename "$0") [-r <repository>] -t <sdk@version> <package_path>

Options:
  -r, --repository     Repository path
  -t, --target         Target in format 'sdk@version', e.g., 'java@1.8'
  -h, --help           Show this help message

Examples:
  $(basename "$0") -t java@1.8 /home/user/packages/java-1.8.tar.gz
  $(basename "$0") -r /path/to/repo -t python@3.11 /tmp/python-3.11.zip
EOF
}

# ----------------------------------------------------------------------------
# 错误退出
# ----------------------------------------------------------------------------
die() {
    echo "Error: $*" >&2
    exit 1
}


# ----------------------------------------------------------------------------
# 解析参数
# ----------------------------------------------------------------------------
parse_args() {
    REPO=""
    TARGET=""
    PACKAGE_PATH=""

    while [[ $# -gt 0 ]]; do
        case "$1" in
            -r|--repository)
                if [[ -z "${2:-}" ]]; then
                    die "Option $1 requires an argument"
                fi
                REPO="$2"
                shift 2
                ;;
            -t|--target)
                if [[ -z "${2:-}" ]]; then
                    die "Option $1 requires an argument"
                fi
                TARGET="$2"
                shift 2
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            -*)
                die "Unknown option: $1"
                ;;
            *)
                if [[ -n "$PACKAGE_PATH" ]]; then
                    die "Multiple package paths are not supported"
                fi
                PACKAGE_PATH="$1"
                shift
                ;;
        esac
    done

    if [[ -z "$TARGET" ]]; then
        die "Target (-t) is required"
    fi
    if [[ -z "$PACKAGE_PATH" ]]; then
        die "Package path is required"
    fi
    if [[ ! -f "$PACKAGE_PATH" ]]; then
        die "Package file not found: $PACKAGE_PATH"
    fi
}

# ----------------------------------------------------------------------------
# 确定仓库路径
# ----------------------------------------------------------------------------
resolve_repo() {
    if [[ -n "$REPO" ]]; then
        echo "$REPO"
    else
        local sdk_path
        sdk_path=$(vfox config storage.sdkPath 2>/dev/null) || true
        if [[ -n "$sdk_path" ]]; then
            echo "$sdk_path"
        else
            echo "$DEFAULT_REPO"
        fi
    fi
}



# ----------------------------------------------------------------------------
# 解析 target (sdk@version)
# ----------------------------------------------------------------------------
parse_target() {
    local target="$1"
    IFS='@' read -r SDK VERSION <<< "$target"
    if [[ -z "$SDK" || -z "$VERSION" ]]; then
        die "Invalid target format: $target. Expected 'sdk@version'."
    fi
}

# ----------------------------------------------------------------------------
# 构建目标路径
# ----------------------------------------------------------------------------
build_target_dir() {
    local repo="$1" sdk="$2" version="$3"
    echo "${repo}/${sdk}/v-${version}/${sdk}-${version}/"
}

# ----------------------------------------------------------------------------
# 检测压缩包格式，返回类型: tar | zip
# ----------------------------------------------------------------------------
detect_format() {
    local filepath="$1"
    local filename
    filename=$(basename "$filepath")

    case "$filename" in
        *.tar.gz|*.tgz)
            echo "tar"
            ;;
        *.tar)
            echo "tar"
            ;;
        *.zip)
            echo "zip"
            ;;
        *)
            # 尝试用 tar 检测
            if tar -tf "$filepath" &>/dev/null; then
                echo "tar"
            elif unzip -l "$filepath" &>/dev/null; then
                echo "zip"
            else
                die "Unsupported archive format: $filename. Only tar and zip are supported."
            fi
            ;;
    esac
}


# ----------------------------------------------------------------------------
# 查找最佳解压根目录
#
# 从 tmp 目录开始，逐层统计每个目录的直接子项数（文件+目录，含隐藏文件），
# 返回第一个子项数 > 1 的目录路径。
# 如果所有目录的子项数都 <= 1，返回 tmpdir 表示回退到整个包。
# ----------------------------------------------------------------------------
find_extract_root() {
    local tmpdir="$1"
    local current="$tmpdir"

    while true; do
        # 使用 glob 收集直接子项（含隐藏文件，排除 . 和 ..）
        local -a items=()
        shopt -s dotglob nullglob
        items=("$current"/*)
        shopt -u dotglob nullglob

        local count=${#items[@]}

        if [[ $count -gt 1 ]]; then
            echo "$current"
            return 0
        elif [[ $count -eq 1 ]]; then
            # 唯一子项是目录则进入，是文件则当前目录即为提取根
            if [[ -d "${items[0]}" ]]; then
                current="${items[0]}"
            else
                echo "$current"
                return 0
            fi
        else
            # 空目录，回退到 tmp
            echo "$tmpdir"
            return 0
        fi
    done
}


# ----------------------------------------------------------------------------
# 解压并提取内容到目标路径
# 流程：解压到 $target_dir/tmp -> 分析目录结构 -> 移动目标内容 -> 清理 tmp
# ----------------------------------------------------------------------------
extract_to_target() {
    local filepath="$1" format="$2" target_dir="$3"

    # 如果 tmp/ 存在则删除，然后创建
    local tmpdir
    tmpdir="${target_dir}/tmp"
    rm -rf "$tmpdir"
    mkdir -p "$tmpdir"

    # 解压到临时目录
    if [[ "$format" == "tar" ]]; then
        tar -xf "$filepath" -C "$tmpdir"
    else
        unzip -o "$filepath" -d "$tmpdir"
    fi

    # 分析目录结构，查找提取根
    local extract_root
    extract_root=$(find_extract_root "$tmpdir")

    # 移动提取根内容到目标路径
    mv "$extract_root"/* "$target_dir"/ 2>/dev/null || true
    # 处理隐藏文件（排除 . 和 ..）
    for hidden in "$extract_root"/.*; do
        local basename="${hidden##*/}"
        [[ "$basename" == "." || "$basename" == ".." ]] && continue
        mv "$hidden" "$target_dir"/ 2>/dev/null || true
    done

    # 清理临时目录
    rm -rf "$tmpdir"
}


# ----------------------------------------------------------------------------
# 主流程
# ----------------------------------------------------------------------------
main() {
    parse_args "$@"

    local repo
    repo=$(resolve_repo)

    parse_target "$TARGET"

    local target_dir
    target_dir=$(build_target_dir "$repo" "$SDK" "$VERSION")

    # 创建目标目录（如果已存在则继续，tmp 会在 extract_to_target 中删除重建）
    mkdir -p "$target_dir"

    # 检测格式
    local format
    format=$(detect_format "$PACKAGE_PATH")

    # 解压（内部包含分析、移动、清理流程）
    echo "Extracting $PACKAGE_PATH to $target_dir"
    extract_to_target "$PACKAGE_PATH" "$format" "$target_dir"
    echo "Done."
}

main "$@"