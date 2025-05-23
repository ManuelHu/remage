#!/bin/python

"""Convert an output file of remage-doc-dump to a rst file."""

from __future__ import annotations

import math
import re
import sys
from pathlib import Path

if len(sys.argv) < 2:
    msg = "need to pass an original file"
    raise ValueError(msg)

path = sys.argv[1]

outlines = [
    "Macro commands",
    "=" * 31,
    "",
    "```{contents} Command table",
    ":class: this-will-duplicate-information-and-it-is-still-useful-here",
    ":local:",
    "```",
    "",
]
with Path(path).open() as infile:
    inlines = [line.strip("\n") for line in infile]


def remove_whitespace_lines_end(lines: list):
    for i in range(len(lines) - 1, 0, -1):
        if lines[i].strip() not in ("", "::"):
            break
        del lines[i]


idx = 0
in_cmdblock = False
in_guidance = False
lastlevel = -1
leveldiff = 0
last_param = -1  # line number of last parameter start (i.e. "Parameter : ...")

for line in inlines:
    if re.match(r"Command directory path : /RMG/", line):
        line = "## `" + line.removeprefix("Command directory path : ") + "`"  # noqa: PLW2901
        remove_whitespace_lines_end(outlines)
        outlines.extend(["", line, ""])
        in_cmdblock = True
        lastlevel = -1
        leveldiff = 0
    elif re.match(r"Command /RMG/", line):
        line = "### `" + line.removeprefix("Command ") + "`"  # noqa: PLW2901
        remove_whitespace_lines_end(outlines)
        outlines.extend(["", line, ""])
        in_cmdblock = True
        lastlevel = -1
        leveldiff = 0
    elif in_cmdblock and (line == "Guidance :"):
        in_guidance = True
    elif in_cmdblock and in_guidance and not line.startswith(" "):
        if line.startswith("note: "):
            outlines.extend([":::{note}", line[6:], ":::", ""])
        elif line.strip() == "":
            in_guidance = False
        elif line != "":
            outlines.extend([line, ""])
    elif (
        in_cmdblock
        and line == " Commands : "
        and not inlines[idx + 1].startswith(" " * 3)
    ) or (
        in_cmdblock
        and line == " Sub-directories : "
        and inlines[idx + 1] == " Commands : "
    ):
        # ignore directories with no commands.
        pass
    elif in_cmdblock and line != "":
        in_guidance = False

        ouch = False
        for text in ["Sub-directories", "Commands"]:
            if line.startswith(f" {text} : "):
                outlines.extend(["", f"**{text}:**", ""])
                indent = 0
                ouch = True
        if ouch:
            idx += 1
            continue

        stripped_line = line.lstrip()
        indent = math.ceil((len(line) - len(stripped_line)) / 2)

        monospaced = False
        if line.startswith(" Range of parameters : "):
            monospaced = True
            indent = 0

        if line.startswith("Parameter : "):
            last_param = idx
            monospaced = True

        stripped_line = stripped_line.rstrip()
        if (
            lastlevel == -1 and indent > lastlevel + 1
        ):  # parts of the output have the wrong indentation.
            leveldiff = indent
        indent -= leveldiff
        no_star_prefix = False
        if idx == last_param + 1 and indent == 0:
            # parts of the output have a weird indentation (param guidance).
            indent += 1
            no_star_prefix = True
        m = re.match(r"(.*)( [:* ] ?)(.*)?$", line)
        if m:
            g = list(m.groups())
            sep = g[1].strip()
            fmt = "**" if sep == ":" else "`"
            if len(g) > 1:
                g[0] = f"{fmt}{g[0].strip()}{fmt}"
                g[1] = " –"  # noqa: RUF001
            if len(g) > 2 and g[2] != "":
                fmt2 = "`" if monospaced or fmt == "**" else ""
                g[2] = f" {fmt2}{g[2].strip()}{fmt2}"
            stripped_line = "".join(g)

        star_prefix = "* " if not no_star_prefix else "  – "  # noqa: RUF001
        outlines.append("  " * indent + star_prefix + stripped_line)
        lastlevel = indent
    idx += 1

with Path("rmg-commands.md").open("w") as outfile:
    outfile.writelines([line + "\n" for line in outlines])
