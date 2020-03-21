# -*- coding: utf-8 -*-
import os
import sys
import re
import codecs

# チェック対象の拡張子リスト
extensions = (
	".cpp",
	".h",
)

regAndLine = (
	(re.compile(r'_wcsdup'),   1),
	(re.compile(r'_wtempnam'), 1),
	(re.compile(r'malloc'),    1),
	(re.compile(r'realloc'),   1),
)


# チェック対象の拡張子か判断する
def checkExtension(fileName):
	base, ext = os.path.splitext(fileName)
	return (ext in extensions)

def fixupCode(fileName):
	tmp_file = fileName + ".tmp"
	with codecs.open(tmp_file, "w", "utf_8_sig") as fout:
		with codecs.open(fileName, "r", "utf_8_sig") as fin:
			for line in fin:
				for patternAndNo in regAndLine:
					pattern = patternAndNo[0]
					lineNo  = patternAndNo[1]
					match = pattern.search(line)
					if match:
						fout.write('#line ' + str(lineNo) + '\r\n')
						break

				fout.write(line)

	os.remove(fileName)
	os.rename(tmp_file, fileName)

# 対象のファイルをすべて処理する
def processAllFiles(topDir):
	for rootdir, dirs, files in os.walk(topDir):
		for fileName in files:
			if checkExtension(fileName):
				full = os.path.join(rootdir, fileName)
				fixupCode(full)

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print ("usage: " + os.path.basename(sys.argv[0]) + " <top dir>")
		sys.exit(1)

	processAllFiles(sys.argv[1])
