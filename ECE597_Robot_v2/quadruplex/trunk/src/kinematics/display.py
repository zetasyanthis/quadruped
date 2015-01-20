#!/usr/bin/python
'''
Display the results of applying the specified neural net to
a number of random points.
'''

import os
import sys
from subprocess import Popen, PIPE

from numpy import matrix, asarray, concatenate, linspace
from pybrain.tools.xml.networkreader import NetworkReader
from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D

from train import forward, genSamples

def getLinearTargets((x0, y0, z0), (x1, y1, z1)):
	points = 10
	for (x, y, z) in zip(linspace(x0, x1, num=points),
						linspace(y0, y1, num=points),
						linspace(z0, z1, num=points)):
		yield (x, y, z), None # Actual angles unknown

def subproc(*args):
	stdout, stderr = Popen(args, stdout=PIPE).communicate()
	#print "stdout was", stdout,
	return stdout

def plotLeg(ax, clr, joint1, joint2, joint3, foot): 
	''' 
	Plot a wireframe of the three joints and the foot position. 
	''' 
	joints = asarray(concatenate([ 
			joint1, 
			joint2, 
			joint3, 
			foot,
		], axis=1))
	ax.plot(joints[0], joints[1], zs=joints[2] * -1, color=clr)


if __name__ == '__main__':
	try:
		netFile = sys.argv[1]
		pathType = sys.argv[2]
		if pathType not in ('random', 'line', 'arc'):
			raise ValueError()
	except (IndexError, ValueError):
		sys.stderr.write('usage: {0} <network-file> random|line|arc\n'.format(sys.argv[0]))
		sys.exit(2)
	net = NetworkReader.readFrom(netFile)
	fig = pyplot.figure()
	ax = Axes3D(fig)

	start, end = (0, 9., 5.), (0., 9, 5.)
	if pathType == 'random':
		inputSeq = genSamples(10)
	elif pathType == 'line':
		inputSeq = getLinearTargets(start, end)
	elif pathType == 'arc':
		print "Not yet implemented"
		inputSeq = getArcTargets(start, end)

	for input, target in inputSeq:
		print ">>>>> target angles:", target
		if target is not None:
			plotLeg(ax, 'green', matrix('0;0;0'), *forward(*target))
		legPos = net.activate(input)
		plotLeg(ax, 'blue',  matrix('0;0;0'), *forward(*legPos))
		try:
			cLegPos = map(float, subproc('./inverse_test', *map(str, input)).strip().split())
			plotLeg(ax, 'red', matrix('0;0;0'), *forward(*cLegPos))
		except Exception, e:
			print e
		print "pyBrain angles", legPos
		print "c net angles  ", cLegPos
		#print "difference    ", legPos - cLegPos
	ax.set_xlim3d(-7.0, 20.0)
	ax.set_ylim3d(-20.0, 7.0)
	ax.set_zlim3d(-20.0, 7.0)
	pyplot.show()

