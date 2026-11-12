#!/usr/bin/env bash
# Builds the Canvas source ZIP from the committed files of the current commit.
#
# Usage: bash tools/make-source-zip.sh [OUTPUT.zip]
# Default output: dist/PioneerDelivery-PartIII-source.zip
#
# git archive includes only tracked files, so build directories, logs, operational state and
# credentials cannot enter the ZIP. The pathspec list below also leaves out the internal
# agent-orchestration material (instructions, tickets, worker briefs, templates).
set -eu

repoRoot="$(cd "$(dirname "$0")/.." && pwd)"
cd "${repoRoot}"
mkdir -p dist
output="${1:-dist/PioneerDelivery-PartIII-source.zip}"
prefix="PioneerDelivery-PartIII"

if [ -n "$(git status --porcelain --untracked-files=no)" ]; then
    echo "make-source-zip: error: the working tree has uncommitted changes to tracked files" >&2
    exit 1
fi
if [ ! -f docs/AlgorithmReport.pdf ]; then
    echo "make-source-zip: error: docs/AlgorithmReport.pdf is missing; compile the report first" >&2
    exit 1
fi

rm -f "${output}"
git archive --format=zip --prefix="${prefix}/" -o "${output}" HEAD -- . \
    ':(exclude)CLAUDE.md' \
    ':(exclude)START_HERE.md' \
    ':(exclude)MANIFEST.md' \
    ':(exclude)ORCHESTRATOR_LAUNCH_PROMPT.md' \
    ':(exclude)reference' \
    ':(exclude)docs/agents' \
    ':(exclude)docs/workers' \
    ':(exclude)docs/tickets' \
    ':(exclude)docs/templates' \
    ':(exclude)docs/07_AGENT_TOPOLOGY.md' \
    ':(exclude)docs/08_ISOLATION_AND_SECURITY.md' \
    ':(exclude)docs/09_WORKER_RECOVERY.md' \
    ':(exclude)docs/15_REVIEW_AND_HANDOFF.md' \
    ':(exclude)docs/16_ENVIRONMENT_PREFLIGHT.md' \
    ':(exclude)docs/17_DECISIONS_AND_UNCERTAINTIES.md' \
    ':(exclude)docs/18_TOOLING_REFERENCES.md'

echo "== wrote ${output}"
unzip -l "${output}" | tail -n 3
( cd "$(dirname "${output}")" && sha256sum "$(basename "${output}")" )
