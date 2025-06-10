#!/bin/bash

# This script generates a PVS-Studio report for the specified project directory.
# Usage: ./generate_pvs_studio_rep.sh <project_directory>
# pvs-studio-analyzer trace -- make clean all
# pvs-studio-analyzer analyze -o report.log
# plog-converter -t fullhtml -o report.html report.log

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <project_directory>"
    exit 1
fi
PROJECT_DIR=$1
cd "$PROJECT_DIR" || exit 1
# Run PVS-Studio analyzer
pvs-studio-analyzer trace -- make clean all
# Analyze the results
pvs-studio-analyzer analyze -o report.log
# Convert the report to HTML format
plog-converter -t fullhtml -o report.html report.log
# Check if the report was generated successfully
if [ $? -eq 0 ]; then
    echo "PVS-Studio report generated successfully: report.html"
else
    echo "Failed to generate PVS-Studio report."
    exit 1
fi
