#!/usr/bin/env bash
# Builds the prebuilt Linux quick-start archive for pioneer.
#
# The output is dist/pioneer-linux-x86_64.tar.gz plus a .sha256 file. The archive
# holds a statically linked pioneer binary, a README, and the three example inputs,
# so a user needs no compiler, CMake, or other tools to run it.
#
# Tools used: cmake, ninja, strip, readelf, tar, sha256sum, uname. The script makes no
# network calls and needs no root privileges. Only the x86_64 label is supported.

set -eu

readonly kArchLabel="x86_64"
readonly kPackageName="pioneer-linux-${kArchLabel}"

repoRoot="$(cd "$(dirname "$0")/.." && pwd)"
buildDir="${repoRoot}/build-release"
distDir="${repoRoot}/dist"
stageDir="${distDir}/${kPackageName}"
archivePath="${distDir}/${kPackageName}.tar.gz"
checksumPath="${archivePath}.sha256"

fail() {
    echo "package-linux: error: $*" >&2
    exit 1
}

# Refuse early when a required tool is missing, instead of failing mid-build.
requireTool() {
    if ! command -v "$1" >/dev/null; then
        fail "required tool '$1' is not installed or not on PATH"
    fi
}

for tool in cmake ninja strip readelf tar sha256sum uname; do
    requireTool "${tool}"
done

# The label is fixed to x86_64, so refuse to package on any other CPU rather than
# produce an archive whose name lies about its contents.
machineArch="$(uname -m)"
if [ "${machineArch}" != "${kArchLabel}" ]; then
    fail "this script packages only ${kArchLabel}; this machine reports '${machineArch}'"
fi

# Configure and build only the pioneer target. Test targets are not needed for the
# archive and are not linked statically.
cmake -S "${repoRoot}" -B "${buildDir}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DPIONEER_STATIC=ON
cmake --build "${buildDir}" --target pioneer

# Rebuild the staging folder from scratch so no stale files reach the archive.
# This removes only the dist/<package> folder this script owns.
rm -rf "${stageDir}"
mkdir -p "${stageDir}/examples"

# Strip a copy so the build tree keeps unstripped symbols for debugging.
cp "${buildDir}/pioneer" "${stageDir}/pioneer"
strip "${stageDir}/pioneer"

# Confirm the packaged binary is static. A dynamically linked ELF program has an
# INTERP program header naming its loader; a static one has none. readelf comes with
# the same binutils package as strip, so no extra tool is needed.
if readelf -lW "${stageDir}/pioneer" | grep -q INTERP; then
    fail "packaged binary is dynamically linked (it has an INTERP header)"
fi

cp "${repoRoot}/packaging/QUICKSTART.txt" "${stageDir}/README.txt"
cp "${repoRoot}/examples/schedule.txt" "${stageDir}/examples/schedule.txt"
cp "${repoRoot}/examples/knapsack.txt" "${stageDir}/examples/knapsack.txt"
cp "${repoRoot}/examples/cleanup.txt" "${stageDir}/examples/cleanup.txt"

# Fixed owner, group, mtime, and sorted names make the archive byte-reproducible
# for the same input files.
rm -f "${archivePath}" "${checksumPath}"
tar --owner=0 --group=0 --numeric-owner --sort=name --mtime='2026-01-01 00:00Z' \
    -C "${distDir}" -czf "${archivePath}" "${kPackageName}"

# Write the checksum with a relative file name so it verifies from the dist folder.
(
    cd "${distDir}"
    sha256sum "${kPackageName}.tar.gz" > "${kPackageName}.tar.gz.sha256"
)

echo "== archive listing: ${archivePath}"
tar -tzf "${archivePath}"
echo "== checksum"
cat "${checksumPath}"
