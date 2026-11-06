Import("env")

import subprocess
import sys
from pathlib import Path


# PlatformIO project directory
project_dir = Path(env["PROJECT_DIR"])

print(f"Python interpreter: {sys.executable}")

# GameProject directory
game_project_dir = project_dir.parent

# Locations
proto_dir = game_project_dir / "Protocols" / "Proto"
nanopb_generator = project_dir / "tools" / "nanopb-0.4.9.2-windows-x86" / "generator" / "nanopb_generator.py"
output_dir = project_dir / "src" / "generated"

PROTO_FILES = [
    "hardware.proto",
]


print("======================================")
print("Generating Protocol Buffer files")
print("======================================")

# Make sure output directory exists
output_dir.mkdir(parents=True, exist_ok=True)


# Generate every .proto file
for proto_name in PROTO_FILES:
    proto_file = proto_dir / proto_name

    if not proto_file.exists():
        raise FileNotFoundError(
            f"Proto file does not exist: {proto_file}"
        )
    
    print(f"Generating: {proto_file.name}")

    subprocess.run(
    [
        sys.executable,
        str(nanopb_generator),
        "-I",
        str(proto_dir),
        "-D",
        str(output_dir),
        str(proto_file)
    ],
    check=True
)


print("Protocol Buffer generation complete.")