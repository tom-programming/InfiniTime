import re

with open("output_main.log", "r") as f:
    lines = f.readlines()

lines_with_dollar = [l for l in lines if l.startswith("$")]

def parse_stop_number(gdb_str):
    reg = re.compile(r"\$([0-9]+) = (\d+)")
    line_number, value = re.match(reg, gdb_str).groups()
    return int(line_number), int(value)

ln_values = [parse_stop_number(ld) for ld  in lines_with_dollar]

times = [v for n,v in ln_values if n % 3 == 1]
raws = [v for n,v in ln_values if n % 3 == 2]
ambs = [v for n,v in ln_values if n % 3 == 0]


# registers:
# 8, 13, 14, 9, 10, 15, ... periodic