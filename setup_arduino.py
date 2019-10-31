#!/usr/bin/env python3

import sys
import os
import subprocess

SCRIPT_DIR=os.path.dirname(__file__);   # We want the relative path.
SOURCE_WHITELIST=[
    "alib_cb_funcs.c",
    "alib_string.c",
    "alib_time.c",
    "DList.c",
    "DListItem.c",
    "Endianess.c",
    "ListItem.c",
    "ListItemVal.c",
];

def linkFile(filename, toFilename=None):
    """
        Generates a link file to the given filename.
    """
    if(toFilename == None):
        toFilename = filename;
    toFilename = os.path.join(SCRIPT_DIR, "..", os.path.basename(toFilename));
    # Remove the file if it is a directory, otherwise instead of replacing the file, the link will be placed within the directory.
    if(os.path.isdir(filename) and os.path.exists(toFilename)):
        os.remove(toFilename);

    filename = os.path.join(os.path.basename(os.path.abspath(SCRIPT_DIR)), filename);    # Have to add the project name in front in order for this to work.

    print("Creating file '%s' linking to '%s'."%(toFilename, filename));

    shellCmd = ["ln", "-sf", filename, toFilename];
    subprocess.run(shellCmd);

# Copy single include headers.
linkFile(os.path.join(SCRIPT_DIR, "Arduino_alib-c.h"));
linkFile(os.path.join(SCRIPT_DIR, "includes"));

# Copy source.
baseDir = os.path.join(SCRIPT_DIR, "source");
for rootDir, _, files in os.walk(baseDir):
    fileHeader = rootDir.split(baseDir)[-1];
    if(len(fileHeader) > 0):
        fileHeader = "_".join(fileHeader.split('/')[1:]);
        fileHeader += "_";

    for f in files:
        if(os.path.basename(f) in SOURCE_WHITELIST):
            linkFile(os.path.join(rootDir, f), fileHeader + f);
