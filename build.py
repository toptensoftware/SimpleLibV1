#!/usr/bin/python3
import sys
import os

platforms=[]
targets=[]
bRebuild=0

# Platform configuration
if os.name=="nt":
	# Visual Studio projects
	workspace_file="SimpleLib9.sln"
	project_name="SimpleLibAtlCom"
	build_cmd="devenv.com \"$(SolutionFile)\" /Build \"$(Target)|$(Platform)\" /project \"$(ProjectName)\" /out build.log"
	rebuild_cmd="devenv.com \"$(SolutionFile)\" /Rebuild \"$(Target)|$(Platform)\" /project \"$(ProjectName)\" /out build.log"
elif os.name=="posix":
	# Code::Blocks projects
	workspace_file="SimpleLib.workspace"
	project_name=""
	build_cmd="codeblocks --build --target=\"$(Target)\" \"$(SolutionFile)\""
	rebuild_cmd="codeblocks --rebuild --target=\"$(Target)\" \"$(SolutionFile)\""


# Parse command line arguments
for arg in sys.argv[1:]:
    if arg.lower()=="debug":
        if os.name=="nt":
            targets.append("DS")
            targets.append("DD")
        else:
            targets.append("Debug")

    elif arg.lower()=="release":
        if os.name=="nt":
            targets.append("RS")
            targets.append("RD")
        else:
            targets.append("Release")

    elif arg.lower()=="win32":
        platforms.append("Win32")

    elif arg.lower()=="x64":
        platforms.append("x64")

    elif arg.lower()=="posix":
        platforms.append("posix")

    elif arg.lower()=="rebuild":
        bRebuild=1

    else:
        sys.exit("Unknown command line switch: "+arg);


# If target not specified, build all
if len(targets)==0:
    if os.name=="nt":
        targets.append("DS")
        targets.append("DD")
        targets.append("RS")
        targets.append("RD")
    else:
        targets.append("Debug")
        targets.append("Release")

# If platform not specified, build all
if len(platforms)==0:
    if os.name=="nt":
        platforms.append("Win32")
        platforms.append("x64")
    else:
        platforms.append("posix")


# Build helper
def build(target, platform):

    # Work out command
    if bRebuild:
        cmd=rebuild_cmd
    else:
        cmd=build_cmd

    # Replace variables
    cmd=cmd.replace("$(SolutionFile)", workspace_file)
    cmd=cmd.replace("$(Target)", target)
    cmd=cmd.replace("$(Platform)", platform)
    cmd=cmd.replace("$(ProjectName)", project_name)

    # Execute
    print("Building", target, platform);
    if os.system(cmd)!=0:
        sys.exit("*** SimpleLib Build failed ***");

# Delete previous build log
if os.path.isfile("build.log"):
    os.unlink("build.log")

# Build all specified combinations
for platform in platforms:
    for target in targets:
        build(target, platform)

