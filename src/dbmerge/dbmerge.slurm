#!/bin/bash

#SBATCH --job-name=dbmerge
#SBATCH --output=dbmerge.out
#SBATCH --error=dbmerge.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=10:00:00
#SBATCH --partition=compute
#SBATCH --account=pa240702

# Compile the Linux kernel under csmake
# Actual use with one CPU: Elapsed 14:26:54 MaxRSS 3259932K

set -eu

module load python/3.7.6

# /tmp is 2GB; redirect it here for 64GB
export TMP=$(mktemp -d /dev/shm/dbmerge.XXXXXX)
export TMPDIR="$TMP"

cd ../dbs
../dbmerge/merge.sh

rm -rf "$TMP"
