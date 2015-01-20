#!/usr/bin/python

'''
Fill in an SVG figure template with actual numbers.
'''

import sys
from os.path import dirname, join
sys.path.append(join(dirname(__file__), '../../../src/kinematics'))

from leg_defs import s1, s2, s3

if __name__ == '__main__':
	try:
		templatefn = sys.argv[1]
		outfn = sys.argv[2]
	except IndexError:
		sys.stderr.write('usage: {0} <svg-template> <svg-output>\n'.format(sys.argv[0]))
		sys.exit(2)
	
	with open(templatefn) as templatef:
		with open(outfn, 'w') as outf:
			outf.write(templatef.read().format(
				s1=[float(x) for x in s1],
				s2=[float(x) for x in s2],
				s3=[float(x) for x in s3],
				fmt='.2f', # Format string used for all the dimensions
			))

