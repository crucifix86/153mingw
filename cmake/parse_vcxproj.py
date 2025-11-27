#!/usr/bin/env python3
"""
Parse Visual Studio .vcxproj files and extract information for CMake
"""

import xml.etree.ElementTree as ET
import os
import sys
import re
from pathlib import Path

NS = {'ms': 'http://schemas.microsoft.com/developer/msbuild/2003'}

def parse_vcxproj(vcxproj_path):
    """Parse a .vcxproj file and extract build information"""
    tree = ET.parse(vcxproj_path)
    root = tree.getroot()

    project_dir = os.path.dirname(vcxproj_path)
    project_name = os.path.splitext(os.path.basename(vcxproj_path))[0]

    result = {
        'name': project_name,
        'path': project_dir,
        'sources': [],
        'headers': [],
        'includes': [],
        'defines': [],
        'config_type': 'StaticLibrary'
    }

    # Get configuration type
    for config_type in root.findall('.//ms:ConfigurationType', NS):
        result['config_type'] = config_type.text
        break

    # Get source files (.cpp, .c)
    for compile in root.findall('.//ms:ClCompile', NS):
        inc = compile.get('Include')
        if inc:
            # Normalize path separators
            inc = inc.replace('\\', '/')
            result['sources'].append(inc)

    # Get header files
    for header in root.findall('.//ms:ClInclude', NS):
        inc = header.get('Include')
        if inc:
            inc = inc.replace('\\', '/')
            result['headers'].append(inc)

    # Get include directories (from first ItemDefinitionGroup)
    for item_def in root.findall('.//ms:ItemDefinitionGroup', NS):
        for add_inc in item_def.findall('.//ms:AdditionalIncludeDirectories', NS):
            if add_inc.text:
                dirs = add_inc.text.split(';')
                for d in dirs:
                    d = d.strip().replace('\\', '/')
                    if d and d != '%(AdditionalIncludeDirectories)':
                        # Expand macros
                        d = d.replace('$(SolutionDir)', '../')
                        d = d.replace('$(ProjectDir)', './')
                        result['includes'].append(d)
        break  # Just get first config

    # Get preprocessor definitions
    for item_def in root.findall('.//ms:ItemDefinitionGroup', NS):
        for preproc in item_def.findall('.//ms:PreprocessorDefinitions', NS):
            if preproc.text:
                defs = preproc.text.split(';')
                for d in defs:
                    d = d.strip()
                    if d and d != '%(PreprocessorDefinitions)' and not d.startswith('_'):
                        result['defines'].append(d)
        break

    return result

def generate_cmake(info, output_path=None):
    """Generate CMakeLists.txt content for a project"""

    cmake = []
    cmake.append(f"# Auto-generated from {info['name']}.vcxproj")
    cmake.append("")

    # Determine target type
    if info['config_type'] == 'Application':
        target_type = 'add_executable'
    else:
        target_type = 'add_library'

    # Source files
    cmake.append(f"set({info['name'].upper()}_SOURCES")
    for src in sorted(info['sources']):
        cmake.append(f"    {src}")
    cmake.append(")")
    cmake.append("")

    # Create target
    if info['config_type'] == 'Application':
        cmake.append(f"{target_type}({info['name']} WIN32")
    else:
        cmake.append(f"{target_type}({info['name']} STATIC")
    cmake.append(f"    ${{{info['name'].upper()}_SOURCES}}")
    cmake.append(")")
    cmake.append("")

    # Include directories
    if info['includes']:
        cmake.append(f"target_include_directories({info['name']} PRIVATE")
        for inc in info['includes']:
            cmake.append(f"    {inc}")
        cmake.append(")")
        cmake.append("")

    # Preprocessor definitions
    if info['defines']:
        cmake.append(f"target_compile_definitions({info['name']} PRIVATE")
        for d in info['defines']:
            cmake.append(f"    {d}")
        cmake.append(")")
        cmake.append("")

    return '\n'.join(cmake)

def main():
    if len(sys.argv) < 2:
        print("Usage: parse_vcxproj.py <path_to_vcxproj> [--cmake]")
        sys.exit(1)

    vcxproj_path = sys.argv[1]
    gen_cmake = '--cmake' in sys.argv

    info = parse_vcxproj(vcxproj_path)

    if gen_cmake:
        print(generate_cmake(info))
    else:
        print(f"Project: {info['name']}")
        print(f"Type: {info['config_type']}")
        print(f"Sources: {len(info['sources'])}")
        print(f"Headers: {len(info['headers'])}")
        print(f"Includes: {info['includes']}")
        print(f"Defines: {info['defines'][:5]}...")  # First 5

if __name__ == '__main__':
    main()
