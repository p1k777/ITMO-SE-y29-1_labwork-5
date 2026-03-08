# Laboratory Work 5. ITMO y29 Software Development

**This repository is a copy of the private repository of the first
laboratory work of the first year at ITMO, program -- Software
Development**

## Purpose of the work

ITMO Loops (see `task_README.md`)

## Working with the compiled executable file

*See `task_README.md`*

## Project structure

-   `instruments` --- implementation of instruments (components that
    write raw sound into tracks stored within them)
-   `effects` --- effects applied to already generated sound from
    instrument tracks
-   `fileparser.*` --- parses the ITMO Loops file and creates the
    objects required for sound generation
-   `wavwriter.*` --- writes instrument tracks to `.wav`
-   `defines.*` --- declarations and definitions of useful functions
-   `notes.txt` --- an important file containing note frequencies
