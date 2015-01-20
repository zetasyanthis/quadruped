#!/usr/bin/python
'''
Display 3D plots of the neural net and its training samples.
'''

import os
import sys
import cPickle as pickle
from optparse import OptionParser

import numpy
from pybrain.tools.xml.networkreader import NetworkReader
from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D

from train import forward

def genTargetMesh(step=2.0):
    '''
    Generator for all of the points in 3-space to test the NN.
    '''
    for x in numpy.arange(-15, 15, step):
        for y in numpy.arange(-15, 18, step):
            for z in numpy.arange(3, 20, step):
                yield numpy.array([x, y, z])

def plotError(net, axes3d):
    for target in genTargetMesh():
        result = net.activate(target)
        result_xyz = numpy.asarray(forward(*result)[-1])[:, 0]
        #numpy.array([target, result_xyz])
        xs = [target[0], result_xyz[0]]
        ys = [target[1], result_xyz[1]]
        zs = [target[2], result_xyz[2]]
        axes3d.plot(xs, ys, zs=zs) 

def plotSamples(samples, axes3D):
    xs = []
    ys = []
    zs = []
    for ((x, y, z), output) in samples:
        xs.append(x)
        ys.append(y)
        zs.append(z)
    axes3d.scatter(xs, ys, zs=zs)

if __name__ == '__main__':
    op = OptionParser()
    op.add_option('-n', '--network', metavar='FILE.xml',
            help='A neural network XML file to show the error of')
    op.add_option('-s', '--samples', metavar='FILE.pickle',
            help='A file containing a pickled list of training samples '
                 'to plot')
    op.set_defaults(network_error=None, samples=None)
    options, args = op.parse_args()
    fig = pyplot.figure()
    axes3d = Axes3D(fig)
    if options.network is not None:
        net = NetworkReader.readFrom(options.network)
        print "Plotting network error"
        plotError(net, axes3d)
    if options.samples is not None:
        with open(options.samples) as f:
            samples = pickle.load(f)
        print "Plotting", len(samples), "samples"
        plotSamples(samples, axes3d)
    axes3d.set_xlabel('x')
    axes3d.set_ylabel('y')
    axes3d.set_zlabel('z')
    axes3d.grid()
    pyplot.show()
