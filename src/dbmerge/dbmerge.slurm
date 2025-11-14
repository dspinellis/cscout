#!/bin/bash

#SBATCH --job-name=dbmerge
#SBATCH --output=dbmerge.out
#SBATCH --error=dbmerge.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=40:00:00
#SBATCH --partition=fat
#SBATCH --mem=256G
#SBATCH --account=pa240702

set -eu

# /tmp is 2GB; redirect it here for 64GB
export TMP=$(mktemp -d /dev/shm/dbmerge.XXXXXX)
export TMPDIR="$TMP"

cd ../dbs
../dbmerge/merge.sh 32

rm -rf "$TMP"
