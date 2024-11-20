#!/bin/bash

cd -- "$(dirname "$0")"

../../Premake/premake5 --file=Build.lua gmake2
../../Premake/premake5 --file=Build.lua vscode
