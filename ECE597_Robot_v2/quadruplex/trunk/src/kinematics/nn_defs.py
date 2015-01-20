#!/usr/bin/python
'''
Generate nn_defs.h from the given neural network XML file.
'''

import sys
import re
import itertools
from numpy import reshape
from pybrain.tools.xml.networkreader import NetworkReader

DATATYPE = 'const float'
TEMPLATE = '''\
// Neural network connection coefficients, automatically generated from {source!r}

/// Weights between input and first hidden layer
{datatype} input_to_hidden0[10][3] = {input_to_hidden0};
/// Weights between bias (1) and first hidden layer
{datatype} bias_to_hidden0[10] = {bias_to_hidden0};
/// Weights between first hidden layer and second hidden layer
{datatype} hidden0_to_hidden1[10][10] = {hidden0_to_hidden1};
/// Weights between bias (1) and second hidden layer
{datatype} bias_to_hidden1[10] = {bias_to_hidden1};
/// Weights between second hidden layer and output layer
{datatype} hidden1_to_output[3][10] = {hidden1_to_output};
/// Weights between bias and output layer
{datatype} bias_to_output[3] = {bias_to_output};
'''

def arrayLiteral(array, braceFormat='{{\n\t {0}\n}}'):
	'Convert a 1-dimensional array to a comma-separated string'
	commasep = ', '.join('{0:+.17f}'.format(f) for f in array)
	commasep = re.sub('(.{60}.*?), ', lambda m: m.group(1) + ',\n\t ', commasep)
	return braceFormat.format(commasep)

def matLiteral(size, array):
	'Convert a 2-dimensional matrix to a comma-separated string'
	mat = reshape(array, size)
	rows = (arrayLiteral(r, '{{{0}}}') for r in mat)
	return '{{\n\t{0}\n}}'.format(',\n\t'.join(rows))
	

def translate(source, net, outf):
	for con in itertools.chain(*net.connections.values()):
		if con.inmod is net['bias']:
			if con.outmod is net['hidden0']:
				bias_to_hidden0 = arrayLiteral(con.params)
			elif con.outmod is net['hidden1']:
				bias_to_hidden1 = arrayLiteral(con.params)
			elif con.outmod is net['out']:
				bias_to_output = arrayLiteral(con.params)
			else:
				raise ValueError('Unexpected connection %r' % con)
		elif con.inmod is net['in'] and con.outmod is net['hidden0']:
			input_to_hidden0 = matLiteral((con.outdim, con.indim), con.params)
		elif con.inmod is net['hidden0'] and con.outmod is net['hidden1']:
			hidden0_to_hidden1 = matLiteral((con.outdim, con.indim), con.params)
		elif con.inmod is net['hidden1'] and con.outmod is net['out']:
			hidden1_to_output = matLiteral((con.outdim, con.indim), con.params)
		else:
			raise ValueError('Unexpected connnection %r' % con)
	# We'll get a KeyError if any of the connections are missing
	datatype = DATATYPE
	try:
		outf.write(TEMPLATE.format(**locals()))
	except KeyError, e:
		raise ValueError('Missing connection: %r' % e)

if __name__ == '__main__':
	try:
		netfile = sys.argv[1]
		headerfile = sys.argv[2]
	except IndexError:
		sys.stderr.write('usage: {0} <network-file> <header-file>\n'.format(sys.argv[0]))
		sys.exit(2)
	net = NetworkReader.readFrom(netfile)
	with open(headerfile, 'w') as outf:
		translate(netfile, net, outf)

