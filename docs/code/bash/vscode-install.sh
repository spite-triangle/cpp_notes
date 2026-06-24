#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"

usage() {
    cat <<EOF
Usage: server_install.sh [OPTIONS] <commit>

Install VSCode Linux Server from local .tar.gz archives.

Options:
  --dir <path>       Directory containing .tar.gz files (default: script directory)
  --mode <mode>      Installation mode: stable or insider (default: stable)
  --clear            Remove installed files for the specified commit
  --force            Force overwrite (install) or suppress warnings (clear)
  -h, --help         Show this help message

Arguments:
  <commit>           VSCode commit hash (40 hex characters)
EOF
}

# Variables set by parse_args
ARG_DIR=""
ARG_MODE="stable"
ARG_CLEAR=false
ARG_FORCE=false
ARG_COMMIT=""

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --dir)
                ARG_DIR="$2"
                shift 2
                ;;
            --mode)
                ARG_MODE="$2"
                shift 2
                ;;
            --clear)
                ARG_CLEAR=true
                shift
                ;;
            --force)
                ARG_FORCE=true
                shift
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            -*)
                echo "Error: Unknown option: $1" >&2
                usage >&2
                exit 1
                ;;
            *)
                if [[ -n "$ARG_COMMIT" ]]; then
                    echo "Error: Multiple commit values provided" >&2
                    exit 1
                fi
                ARG_COMMIT="$1"
                shift
                ;;
        esac
    done
}

parse_args "$@"

# Default --dir to script directory (only used in install mode)
if [[ -z "$ARG_DIR" ]]; then
    ARG_DIR="$SCRIPT_DIR"
fi


validate_commit() {
    local commit="$1"
    if [[ ! "$commit" =~ ^[a-f0-9]{40}$ ]]; then
        echo "Error: Invalid commit hash: $commit (expected 40 hex characters)" >&2
        exit 1
    fi
}

# Validate required arguments
if [[ -z "$ARG_COMMIT" ]]; then
    echo "Error: <commit> is required" >&2
    usage >&2
    exit 1
fi

validate_commit "$ARG_COMMIT"

if [[ "$ARG_MODE" != "stable" && "$ARG_MODE" != "insider" ]]; then
    echo "Error: --mode must be 'stable' or 'insider', got: $ARG_MODE" >&2
    exit 1
fi

if [[ "$ARG_CLEAR" != true && ! -d "$ARG_DIR" ]]; then
    echo "Error: Directory does not exist: $ARG_DIR" >&2
    exit 1
fi

# Mode-specific variables
BASE_DIR=""
CLI_FILE_NAME=""
TARGET_CLI_NAME=""
SERVER_DIR_NAME=""

set_mode_vars() {
    if [[ "$ARG_MODE" == "stable" ]]; then
        BASE_DIR="$HOME/.vscode-server"
        CLI_FILE_NAME="code"
        TARGET_CLI_NAME="code-${ARG_COMMIT}"
        SERVER_DIR_NAME="${ARG_COMMIT}"
    else
        BASE_DIR="$HOME/.vscode-server-insiders"
        CLI_FILE_NAME="code-insiders"
        TARGET_CLI_NAME="code-insiders-${ARG_COMMIT}"
        SERVER_DIR_NAME="Insiders-${ARG_COMMIT}"
    fi
}

TMP_DIR=""

cleanup() {
    if [[ -n "$TMP_DIR" && -d "$TMP_DIR" ]]; then
        rm -rf "$TMP_DIR"
    fi
}

install() {
    local cli_tar="$ARG_DIR/vscode_cli_alpine_x64_cli.tar.gz"
    local server_tar="$ARG_DIR/vscode-server-linux-x64.tar.gz"

    local target_cli_path="$BASE_DIR/$TARGET_CLI_NAME"
    local target_server_path="$BASE_DIR/cli/servers/$SERVER_DIR_NAME"

    # Check if targets already exist
    if [[ -e "$target_cli_path" || -e "$target_server_path" ]]; then
        if [[ "$ARG_FORCE" == true ]]; then
            echo "Force mode: removing existing installation..."
            rm -rf "$target_cli_path" "$target_server_path"
        else
            echo "Error: Target already exists. Use --force to overwrite." >&2
            echo "  $target_cli_path" >&2
            echo "  $target_server_path" >&2
            exit 1
        fi
    fi

    # Check that HOME is writable (BASE_DIR is under HOME)
    if [[ ! -w "$HOME" ]]; then
        echo "Error: Home directory $HOME is not writable" >&2
        exit 1
    fi

    # Ensure BASE_DIR exists before creating temp directory inside it
    mkdir -p "$BASE_DIR"

    # Create temp directory on same filesystem as target
    # TMP_DIR is initialized as empty string at script top; cleanup() checks -n before use
    TMP_DIR=$(mktemp -d "${BASE_DIR}/.vscode-install-XXXXXX")
    trap cleanup EXIT

    # Extract CLI archive to tmp_cli
    local tmp_cli="$TMP_DIR/tmp_cli"
    mkdir "$tmp_cli"
    tar -xzf "$cli_tar" -C "$tmp_cli"

    # Validate CLI file exists in archive
    if [[ ! -e "$tmp_cli/$CLI_FILE_NAME" ]]; then
        echo "Error: Expected '$CLI_FILE_NAME' not found in CLI archive" >&2
        exit 1
    fi

    # Extract server archive to tmp_server
    local tmp_server="$TMP_DIR/tmp_server"
    mkdir "$tmp_server"
    tar -xzf "$server_tar" -C "$tmp_server"
    # Find server directory using glob, validate exactly one match
    shopt -s nullglob
    local server_dirs=("$tmp_server"/vscode-server-*/)
    shopt -u nullglob
    if [[ ${#server_dirs[@]} -ne 1 ]]; then
        echo "Error: Expected exactly one vscode-server-*/ directory in server archive, found ${#server_dirs[@]}" >&2
        exit 1
    fi
    local server_src="${server_dirs[0]}"
    if [[ ! -d "$server_src" ]]; then
        echo "Error: vscode-server-*/ is not a directory" >&2
        exit 1
    fi

    # All validations passed — now move files to target locations
    # NOTE: target_server_path was already removed in --force branch above (or doesn't exist),
    # so mv will rename the directory to 'server' rather than moving it inside an existing 'server/'.
    mkdir -p "$target_server_path"
    mv "$server_src" "$target_server_path/server"
    mv "$tmp_cli/$CLI_FILE_NAME" "$target_cli_path"

    # Cleanup
    trap - EXIT
    cleanup

    echo "Installation complete:"
    echo "  $target_cli_path"
    echo "  $target_server_path/server"
}


clear_install() {
    local target_cli_path="$BASE_DIR/$TARGET_CLI_NAME"
    local target_server_path="$BASE_DIR/cli/servers/$SERVER_DIR_NAME"

    local found=false

    if [[ -e "$target_cli_path" ]]; then
        rm -rf "$target_cli_path"
        echo "Removed: $target_cli_path"
        found=true
    fi

    if [[ -e "$target_server_path" ]]; then
        rm -rf "$target_server_path"
        echo "Removed: $target_server_path"
        found=true
    fi

    if [[ "$found" == false ]]; then
        if [[ "$ARG_FORCE" == true ]]; then
            # --force suppresses "not found" warnings in clear mode
            :
        else
            echo "Warning: No installation found for commit $ARG_COMMIT ($ARG_MODE mode)" >&2
        fi
    fi
}

check_prerequisites() {
    # Check tar is available
    if ! command -v tar &>/dev/null; then
        echo "Error: 'tar' command not found" >&2
        exit 1
    fi

    local cli_tar="$ARG_DIR/vscode_cli_alpine_x64_cli.tar.gz"
    local server_tar="$ARG_DIR/vscode-server-linux-x64.tar.gz"

    if [[ ! -f "$cli_tar" ]]; then
        echo "Error: File not found: $cli_tar" >&2
        exit 1
    fi

    if [[ ! -f "$server_tar" ]]; then
        echo "Error: File not found: $server_tar" >&2
        exit 1
    fi
}

main() {
    set_mode_vars

    if [[ "$ARG_CLEAR" == true ]]; then
        clear_install
    else
        check_prerequisites
        install
    fi
}

main