from os import listdir

files = filter(lambda p: not p.startswith('.') and p.endswith('.h'), listdir('.'))
for f in files:
    print(f)

lines = []
with open('synthesize.c.template', 'r') as templ:
    lines = map(str.strip, templ.readlines())
    lines = filter(lambda s: s.startswith('++'), lines)
    lines = map(lambda s: s.replace('++', '', 1), lines)
    lines = list(lines)

for line in lines:
    with open(line, 'r') as f:
        print(f.read())
    
