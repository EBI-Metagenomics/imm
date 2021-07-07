import json
import os
import subprocess
from pathlib import Path
from shutil import which
from urllib.request import urlopen


def fetch_file():
    url = "ftp://ftp.ncbi.nih.gov/entrez/misc/data/gc.prt"
    rows = []
    for row in urlopen(url):
        r = row.decode()
        if not r.startswith("--"):
            r = r.strip()
            if len(r) == 0:
                continue
            if r.startswith("Genetic-code-table"):
                continue
            rows.append(r)
    rows = rows[:-1]
    return rows


def fix_newline_between_quotes(rows):
    open_quote = False
    new_rows = []
    for r in rows:
        if not open_quote:
            new_rows.append(r)
        else:
            new_rows[-1] += " " + r
        open_quote ^= r.count("\"") % 2 == 1
    return new_rows


def make_json(rows):
    new_rows = []
    name_idx = 1
    for r in rows:
        if r.startswith("name "):
            r = r.replace("name ", f"\"name{name_idx}\":")
            name_idx += 1
        if r.startswith("id "):
            r = r.replace("id ", "\"id\":\"").strip()
            r = r.replace(" ,", "\",")
            name_idx = 1
        r = r.replace(" ,", ",")
        r = r.replace("ncbieaa  ", "\"ncbieaa\":")
        if r.startswith("sncbieaa"):
            r = r.replace("sncbieaa ", "\"sncbieaa\":")
            r += ","
        if r.startswith("-- Base"):
            r = r.replace("-- Base1  ", "\"base1\":\"")
            r = r.replace("-- Base2  ", "\"base2\":\"")
            r = r.replace("-- Base3  ", "\"base3\":\"")
            if r.startswith("\"base3\""):
                r += "\""
            else:
                r += "\","
        new_rows.append(r)

    new_rows.insert(0, "[")
    new_rows.append("]")
    return new_rows


def remove_name2(data):
    new_data = []
    for d in data:
        d["name2"] = d.get("name2", "")
        new_data.append(d)
    return new_data


def genetic_code():
    rows = fix_newline_between_quotes(fetch_file())
    rows = make_json(rows)
    gc = json.loads("\n".join(rows))
    gc = remove_name2(gc)
    return gc


def is_tool(name):
    """Check whether `name` is on PATH and marked as executable."""
    return which(name) is not None


def prj_dir():
    prj = Path(os.path.dirname(os.path.realpath(__file__))).parent
    cmakelists = prj / "CMakeLists.txt"

    if not (cmakelists.exists() and cmakelists.is_file()):
        raise RuntimeError("Failed to find project dir.")

    return prj


def src_dir(prj: Path):
    src = prj / "src"
    if not (src.exists() and src.is_dir()):
        raise RuntimeError("Failed to find source dir.")
    return src


def write_c_file(src: Path, gc):
    print(f"Generating {src}... ", end="")
    with open(src, "w") as file:
        file.write("#include \"gc.h\"\n\n")
        file.write("struct gc gc[] = {\n")
        for d in gc:
            name1 = d["name1"]
            name2 = d["name2"]
            _id = d["id"]
            ncbieaa = d["ncbieaa"]
            sncbieaa = d["sncbieaa"]
            base1 = d["base1"]
            base2 = d["base2"]
            base3 = d["base3"]
            file.write(f"    {{\"{name1}\", \"{name2}\", {_id},\n")
            file.write(f"     \"{ncbieaa}\",\n")
            file.write(f"     \"{sncbieaa}\",\n")
            file.write(f"     \"{base1}\",\n")
            file.write(f"     \"{base2}\",\n")
            file.write(f"     \"{base3}\"}},\n")
        file.write("};\n")
    print("done.")


def format_c_file(prj: Path, src: Path):
    if not is_tool("clang-format"):
        raise RuntimeError("clang-format not found.")

    cmd = f"clang-format -i {src}"
    print(f"Running `{cmd}`... ", end="")
    subprocess.check_call(cmd, cwd=prj, shell=True)
    print("done.")


if __name__ == "__main__":
    gc = genetic_code()
    prj = prj_dir()
    src = src_dir(prj) / "gc_auto.c"
    write_c_file(src, gc)
    format_c_file(prj, src)
