#!/bin/bash 
make TOOL=gcc_arm CONFIG=debug LOAD=intflash build
make TOOL=gcc_arm CONFIG=release LOAD=intflash build
