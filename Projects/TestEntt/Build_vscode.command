#!/bin/bash

cd -- "$(dirname "$0")"

../../Premake/premake5 --file=premake5.lua vscode
