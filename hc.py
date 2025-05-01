import sys
import subprocess

def build_usd(variant):
    options = [
        "--build-variant", variant,
        "--no-examples",
        "--no-tutorials",
        "--no-tools",
        "--no-tools",
        "--no-python",
        "--no-usdValidation",
        "--imaging",
        "--no-usdview",
    ]
    subprocess.run([sys.executable, "build_scripts/build_usd.py", *options, "../../../../out/builds/usd"], cwd="Code/External/OpenUSD/OpenUSD", check=True)


def build_usd_dbg():
    build_usd(variant="debug")


def setup():
    args = [
        "--fresh", 
        "--preset", "vs-win-amd64", 
        "-Dpxr_DIR=out/builds/usd", 
        "-DOpenSubdiv_DIR=out/builds/usd/lib/cmake/OpenSubdiv", # TODO: There is an issue with usd install, OpenSubdiv is not found unless I set this path
    ]
    subprocess.run(["cmake", *args], check=True)


def build():
    args = [
        "--build", 
        "--preset", "vs-win-amd64",
        "--config", "Debug",
    ]
    subprocess.run(["cmake", *args], check=True)


build_usd_dbg()
setup()
#build()
