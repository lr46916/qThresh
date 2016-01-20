# qThresh

In this project we implemented a program to calculate optimal threshold values for gapped q-grams as presented in:
http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.13.5942&rep=rep1&type=pdf

Compiling
---------------------------------------------------------------------

In order to compile out program (to get an executable) position yourself in the folder named
`findThresholdMultithread` and invoke `make`.

(for more information about make command follow "http://www.tutorialspoint.com/unix_commands/make.htm")

The only dependency required to compile the program is a working version of g++ (https://gcc.gnu.org/onlinedocs/gcc/G_002b_002b-and-GCC.html) which supports the C++11 standard.

Once compilation is complete you should see the executable, named `findAllThresholds`.

The program has been tested to work on both Linux and Windows operating systems.

---------------------------------------------------------------------

Running
---------------------------------------------------------------------

To run the program You need to execute the previously compiled executable, named `findAllThresholds`. In order to run a program several arguments needs to be passed trough command line. 

There are 3 important parameters for this problem: `m`, `k` and `span`.
`m` - the size of the search pattern,
`k` - the hamming distance between the pattern and matches,
`span` - only q-grams with span smaller (or equal) to this parameter value will be considered in search space

Command line arguments to run the program are as follows:

First param -> integer value for parameter `m`.

Second param -> integer value for parameter `k`.

Third param -> integer value. Maximal span of a q-gram.

Fourth param -> File in which results will be stored.

Fifth param -> Resume file to be saved. The file in which current iteration will be stored.

Sixth param -> Resume file to be read. If none provided computation starts from `q`=1.

Example
---------------------------------------------------------------------

To start computing all positive thresholds for m=50, k=5 and span=20 run:

`./findAllThresholds 50 5 20 results.txt resume.txt`

Once computation is complete results will be stored in "results.txt" file.
More about results file format can be found later in this README file.

"resume.txt" file is a file in which information from which we can resume computation from some given point will be stored.
If we provide such file as a 6-th argument computation will be resumed from that state.
This feature is added due to possibility of long computations.

Result format
---------------------------------------------------------------------

Results file will contain N lines. One per each shape for which program calculated positive threshold (>= 1).
Each line consists of two numbers:
  first - shape represented as a long value (1 represents a matching character, 0 a gap)
  second - threshold of that shape

In order to obtain a more (humanly) readable file format we added a python script, `solutionFormatter.py`.

Now each shape is represented as a string of "#" and "-" symbol.
Symbol "#" indicates a character that will be considered for matching on that position and "-" indicates a gap.

Example of using "solutionFormatter.py" script:
  python solutionFormatter.py results.txt resultsPretty.txt t

First command line argument is a file which contains results which are to be reformatted

Second command line argument is a file in which reforamtted results will be stored.

Third command line argument is a key (option) which will be used to sort shapes before saving them to output file.

There are 3 key options: `t`, `s` and `q`:

  `t` - sorts shapes by threshold values, descending

  `s` - sorts shapes by span values, ascending

  `q` - sorts shapes by q values (shape size), ascending

