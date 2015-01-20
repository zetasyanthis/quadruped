'''
Generate a C header file with a feedforward neural network definition,
given the network's dimensions.
'''

def main(outfn, sizes):
   with open(outfn, 'w+') as out:
      inputs = sizes[0]
      outputs = sizes[-1]
      hidden = sizes[1:-1]
      out.write('''// AUTOMATICALLY GENERATED -- DO NOT EDIT

      void nn_solve({0}, {1});
      void nn_train({0}, {2});
      '''.format(
         ', '.join('gfloat input_{0}'.format(i) for i in range(inputs)),
         ', '.join('gfloat *output_{0}'.format(i) for i in range(outputs)),
         ', '.join('gfloat output_{0}'.format(i) for i in range(outputs))
      ))
      # TODO: Generate static data structures for net's state:q
      #


if __name__ == '__main__':
   main('nn.h', [3, 10, 10, 3])


