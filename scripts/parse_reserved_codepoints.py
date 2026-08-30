import sys

def parse_unicode_data(filepath: str) -> set:
    assigned_cps = set()
    range_start = None

    with open(filepath, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            fields = line.split(";")
            cp = int(fields[0], 16)
            name = fields[1]

            if name.endswith(", First>"):
                range_start = cp
            elif name.endswith(", Last>"):
                if range_start is not None:
                    for r_cp in range(range_start, cp + 1):
                        assigned_cps.add(r_cp)
                    range_start = None
            else:
                assigned_cps.add(cp)

    return assigned_cps

def is_noncharacter_or_surrogate(cp: int) -> bool:
    # Surrogate area in BMP
    if 0xD800 <= cp <= 0xDFFF:
        return True
    # Noncharacters ending in FFFE or FFFF
    if (cp & 0xFFFF) in (0xFFFE, 0xFFFF):
        return True
    # Noncharacter block in BMP
    if 0xFDD0 <= cp <= 0xFDEF:
        return True
    return False

def generate_table(name: str, codepoints: list, mask_16bit: bool = False) -> str:
    lines = []
    lines.append(f"// Size: {len(codepoints)} entries")
    lines.append(f"const uint16_t {name}[{len(codepoints)}] = {{")

    chunk_size = 8
    for i in range(0, len(codepoints), chunk_size):
        chunk = codepoints[i:i + chunk_size]
        if mask_16bit:
            formatted = ", ".join(f"0x{cp & 0xFFFF:04X}" for cp in chunk)
        else:
            formatted = ", ".join(f"0x{cp:04X}" for cp in chunk)
        lines.append(f"    {formatted},")

    lines.append("};\n")
    return "\n".join(lines)

def main():
    ucd_path = "UnicodeData.txt"

    try:
        assigned_cps = parse_unicode_data(ucd_path)
    except FileNotFoundError:
        print(f"Error: Could not find '{ucd_path}'. Please check the file path.", file=sys.stderr)
        sys.exit(1)

    # BMP range: U+0000..U+FFFF
    bmp_reserved = []
    for cp in range(0x0000, 0x10000):
        if cp not in assigned_cps and not is_noncharacter_or_surrogate(cp):
            bmp_reserved.append(cp)

    # SMP range: U+10000..U+1FFFF
    smp_reserved = []
    for cp in range(0x10000, 0x20000):
        if cp not in assigned_cps and not is_noncharacter_or_surrogate(cp):
            smp_reserved.append(cp)

    file_content = [
        "#ifndef UCS_RESERVED_CODES_HXX",
        "#define UCS_RESERVED_CODES_HXX",
        "#include <cstdint>",
        "#include <cstddef>",
        "",
        "// Generated from UnicodeData.txt",
        f"const size_t NUM_BMP_RESERVED = {len(bmp_reserved)};",
        f"const size_t NUM_SMP_RESERVED = {len(smp_reserved)};",
        "",
        generate_table("BMP_RESERVED_CODEPOINTS", bmp_reserved, mask_16bit=False),
        generate_table("SMP_RESERVED_LOWER16_CODEPOINTS", smp_reserved, mask_16bit=True),
        "#endif // UCS_RESERVED_CODES_HXX"
    ]
    output_filename = "../src/hunspell/ucs_reserved_codes.hxx"
    with open(output_filename, "w", encoding="utf-8") as f:
        f.write("\n".join(file_content))

    print(f"Generated {output_filename}")
    print(f"BMP Reserved entries: {len(bmp_reserved)}")
    print(f"SMP Reserved entries: {len(smp_reserved)}")

if __name__ == "__main__":
    main()
