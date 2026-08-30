#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
UnicodeData.txt parser script for the ucsspell project.
Generates 16-bit lookup tables (0x10000 - 0x1FFFF range) for O(1) 
uppercase and lowercase conversion in the SMP area.
"""

import os

LOCAL_FILE = "UnicodeData.txt"
SMP_TABLE_SIZE = 65536

def build_smp_tables():
    """
    Builds ucs_to_lower and ucs_to_upper mapping arrays 
    for code points in the 0x10000..0x1FFFF range.
    """
    # Default identity mapping: ucs_to_lower[i] = i, ucs_to_upper[i] = i
    ucs_to_lower = list(range(SMP_TABLE_SIZE))
    ucs_to_upper = list(range(SMP_TABLE_SIZE))

    with open(LOCAL_FILE, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            fields = line.split(";")
            if len(fields) < 14:
                continue

            code_hex = fields[0]
            code_point = int(code_hex, 16)

            # Process only the Plane 1 SMP range (0x10000 - 0x1FFFF)
            if 0x10000 <= code_point <= 0x1FFFF:
                index = code_point - 0x10000
                uppercase_hex = fields[12]
                lowercase_hex = fields[13]

                # If an explicit uppercase mapping exists in SMP
                if uppercase_hex:
                    upper_cp = int(uppercase_hex, 16)
                    if 0x10000 <= upper_cp <= 0x1FFFF:
                        ucs_to_upper[index] = upper_cp - 0x10000

                # If an explicit lowercase mapping exists in SMP
                if lowercase_hex:
                    lower_cp = int(lowercase_hex, 16)
                    if 0x10000 <= lower_cp <= 0x1FFFF:
                        ucs_to_lower[index] = lower_cp - 0x10000

    return ucs_to_lower, ucs_to_upper

def generate_header_file(ucs_to_lower, ucs_to_upper, output_file="../src/hunspell/ucs_info.hxx"):
    """Generates the ucs_info.hxx header file containing the char16_t arrays."""
    with open(output_file, "w", encoding="utf-8") as f:
        f.write("#ifndef UCS_INFO\n")
        f.write("#define UCS_INFO\n\n")
        f.write("#include <cstdint>\n\n")
        f.write("/* SMP uppercase/lowercase offsets, indexes and values are offsets from 0x10000 codepoint */\n")

        # Generate ucs_to_lower
        f.write(f"static char16_t ucs_to_lower[{SMP_TABLE_SIZE}] = {{\n")
        for i, val in enumerate(ucs_to_lower):
            comma = "," if i < SMP_TABLE_SIZE - 1 else ""
            f.write(f" 0x{val:04x}{comma}\n")
        f.write("};\n\n")

        # Generate ucs_to_upper
        f.write(f"static char16_t ucs_to_upper[{SMP_TABLE_SIZE}] = {{\n")
        for i, val in enumerate(ucs_to_upper):
            comma = "," if i < SMP_TABLE_SIZE - 1 else ""
            f.write(f" 0x{val:04x}{comma}\n")
        f.write("};\n\n")

        f.write("#endif // UCS_INFO\n")

    print(f"Done! Header successfully created: '{output_file}'.")

if __name__ == "__main__":
    lower_map, upper_map = build_smp_tables()
    generate_header_file(lower_map, upper_map)
