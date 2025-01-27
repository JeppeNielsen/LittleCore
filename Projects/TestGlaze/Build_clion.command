#!/bin/bash

cd -- "$(dirname "$0")"

../../Premake/premake5 --file=Build.lua clion
