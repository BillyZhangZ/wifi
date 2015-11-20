#!/bin/bash
make TOOL=gcc_arm CONFIG=debug LOAD=intflash clean 
make TOOL=gcc_arm CONFIG=release LOAD=intflash clean 
