#!/bin/bash

#SBATCH --job-name=dbmerge
#SBATCH --output=dbmerge.out
#SBATCH --error=dbmerge.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=40:00:00
#SBATCH --partition=fat
#SBATCH --mem=128G
#SBATCH --account=pa240702

set -eu

# Path must include a version of ccomps that avoids issue
# https://gitlab.com/graphviz/graphviz/-/issues/2593
# This can be done by applying write.c.patch

# /tmp is 2GB; redirect it here for 64GB
export TMP=$(mktemp -d /dev/shm/dbmerge.XXXXXX)
export TMPDIR="$TMP"

cd ../dbs
../dbmerge/merge.sh

rm -rf "$TMP"
