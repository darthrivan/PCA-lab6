#!/usr/bin/env python
import sys
import re
from subprocess import Popen, CalledProcessError, PIPE
from os import getcwd

try:
	import matplotlib.pyplot as plt
	import pandas as pd
except ImportError:
	noImport = True

def run(*arg, **opt):
	proc = Popen(*arg, **opt) #if opt is not None else Popen(c)
	retcode = proc.wait()
	if retcode != 0: raise CalledProcessError(retcode, proc)
	return proc

class Debugger(object):
	"""docstring for Debugger"""
	def __init__(self, arg):
		super(Debugger, self).__init__()
		self.arg = arg

	@staticmethod
	def colorize(string, color, out=sys.stdout):
	    colors = {'red': 91 ,'green': 92 ,'yellow': 93, 'blue': 94}
	    if out.isatty():
	        format = '\033[%dm%%s\033[0m' % colors.get(color, 0)
	    else:
	        format = '%s'
	    out.write(format %  string)
	    out.flush()

	@staticmethod
	def log(msg):
		Debugger.colorize(msg, 'blue')
		
	@staticmethod
	def success():
		Debugger.colorize("Done\n", 'green')

	@staticmethod
	def error(msg='', fatal=True):
		error_title = "Fatal Error" if fatal else "Error"
		if len(msg) > 0:
			Debugger.colorize(error_title+": " + msg + "\n", 'red', sys.stderr)
		else:
			Debugger.colorize(error_title+"\n", 'red', sys.stderr)
		if fatal:
			exit(1)

class Program(object):
	"""docstring for Program"""
	def __init__(self, exec_name, path=getcwd(), args=[], input_stream=sys.stdin,
		         output_stream=sys.stdout):
		super(Program, self).__init__()
		self.exec_name = exec_name
		self.path = path
		self.args = args
		self.input_stream = input_stream
		self.output_stream = output_stream

	def get_exec_path(self):
		return self.path+'/'+self.exec_name

	def get_exec_name(self):
		return self.exec_name

	def get_arguments(self):
		return self.args

	def get_argument(self, pos):
		return self.args[pos]

	def get_input_stream(self):
		return self.input_stream

	def get_output_stream(self):
		return self.output_stream

	def set_argument(self, arg, pos=None):
		if pos is None:
			self.args.add(arg)
		else:
			self.args[pos] = arg

	def set_arguments(self, args):
		self.args = args

	def set_input_stream(self, input_stream):
		self.input_stream = input_stream

	def set_output_stream(self, output_stream):
		self.output_stream = output_stream

	def run(self):
		run([self.get_exec_path()]+
			 self.get_arguments(),
			 stdin=self.input_stream,
			 stdout=self.output_stream)
		
class Compiler(object):
	"""docstring for Compiler"""
	def __init__(self, code, flags=[], from_stdin=False):
		super(Compiler, self).__init__()
		self.code = code
		self.flags = flags
		self.from_stdin = from_stdin

	def compile(self, output_name):
		if self.from_stdin:
			Debugger.log("Compiling from stdin...")
			cmd = Popen(['gcc', '-o', output_name, '-x','c'] + self.flags + ['-'],
						stdin=PIPE)
			cmd.communicate(input=self.code)
			if cmd.returncode:
				Debugger.error("Compilation failed")
		else:
			Debugger.log("Compiling " + self.code + "...")
			try:
				run(['gcc', '-o', output_name, self.code] + self.flags)
			except CalledProcessError:
				Debugger.error("Compilation failed")
		Debugger.success()
		return Program(output_name)

	def set_flag(self, option, value=None):
		if value is None:
		    self.flags.append(option)
		else:
			self.flags += [option, value]

class Accounter(object):
	"""docstring for Accounter"""
	def __init__(self, program, numSamples=0):
		super(Accounter, self).__init__()
		self.program = program
		self.numSamples = numSamples
		self.measures = []
		self.df = pd.DataFrame()

	def set_num_samples(self, numSamples):
		self.numSamples = numSamples

	def account(self, stdout_redirect=None):
		Debugger.log("Initiating accounting...")
		for i in range(0, self.numSamples):
			try:
				cmd = run(['/usr/bin/time',
					        #'-o', 'sample.txt', '-a',
					        '-f', '%U,%S,%e,%P,%I,%O,%F,%R,%W',
					        self.program.get_exec_path()
					       ] + self.program.get_arguments(),
					       stdout=stdout_redirect,
					       stderr=PIPE)
				self._add_measure(self._parse(cmd.stderr.read()))
			except CalledProcessError as err:
				Debugger.error("Accounting failed")
		names = ['User', 'System', 'Elapsed',
		         'CPU', 'Inputs', 'Outputs',
		         'MajFaults', 'MinFaults', 'Swaps']
		self.df = pd.DataFrame(data=self.measures, columns=names)
		Debugger.success()

	def _parse(self, str):
		p = lambda e: float(e) if e[-1] != '%' else float(e[:-1])/100.0
		# return [ p(elem) for elem in str[:-1].split(',') ]
		elem = str[:-1].split(',')
		return ( p(elem[0]), p(elem[1]), p(elem[2]),
		         p(elem[3]), p(elem[4]), p(elem[5]),
		         p(elem[6]), p(elem[7]), p(elem[8]) )


	def _add_measure(self, values):
		self.measures.append(values)

	def to_CSV(self, name=None):
		Debugger.log("Exporting samples to csv...")
		if name is None:
			name = 'accounting_'+self.program.get_exec_name()+'.csv'
		self.df.to_csv(name, header=True, index=False)
		Debugger.success()

	def plot(self, column='User'):
		plt.figure()
		self.df[column].hist()
		plt.show()

class Accounter_B(object):
	"""docstring for Accounter_B"""
	def __init__(self, program, num_samples=0):
		super(Accounter_B, self).__init__()
		self.program = program
		self.num_samples = num_samples
		self.measures = []

	def account(self):
		Debugger.log("Initiating accounting...")
		for i in range(0, self.num_samples):
			try:
				cmd = run(['/usr/bin/time',
					        #'-o', 'sample.txt', '-a',
					        '-f', '%U,%S,%e,%P,%I,%O,%F,%R,%W',
					        self.program.get_exec_path()
					       ] + self.program.get_arguments(),
					       stdin=self.program.get_input_stream(),
					       stdout=self.program.get_output_stream(),
					       stderr=PIPE)
				self._add_measure(self._parse(cmd.stderr.read()))
			except CalledProcessError as err:
				Debugger.error("Accounting failed")
		Debugger.success()

	def _parse(self, str):
		p = lambda e: float(e) if e[-1] != '%' else float(e[:-1])/100.0
		# return [ p(elem) for elem in str[:-1].split(',') ]
		elem = str[:-1].split(',')
		return { 'User': p(elem[0]), 'System': p(elem[1]),
				 'Elapsed': p(elem[2]), 'CPU': p(elem[3]),
				 'Inputs': p(elem[4]), 'Outputs': p(elem[5]),
		         'MajFaults': p(elem[6]), 'MinFaults': p(elem[7]),
		         'Swaps': p(elem[8]) }

	def _add_measure(self, values):
		self.measures.append(values)

	def get_measures(self, columns=['User', 'System', 'Elapsed', 'CPU',
		                            'Inputs', 'Outputs', 'MajFaults',
		                            'MinFaults', 'Swaps']):
		f = lambda dic: dict([(k, dic[k]) for k in filter(
			lambda e: e in columns, dic.keys())])
		return map(f, self.measures)
		

class Versioner(object):
	"""docstring for Versioner"""
	def __init__(self, file_path):
		super(Versioner, self).__init__()
		self.file_path = file_path
		self.load_versions()

	def load_versions(self):
		try:
			cmd = run(['git', 'log', '--follow', self.file_path], stdout=PIPE)
			commits = re.findall("(?<=commit )[0-9a-f]{40}", cmd.stdout.read())
		except CalledProcessError:
			Debugger.error("Could not get commits for " + self.file_path)
		self.versions = [ (com, self.read_from_commit(com)) for com in commits ]
		self.versions.reverse()

	def read_from_commit(self, commit):
		try:
			cmd = run(['git', 'show', commit+':'+self.file_path], stdout=PIPE)
			return cmd.stdout.read()
		except CalledProcessError:
			Debugger.error("Could not get committed version")

	def get_versions(self):
		return self.versions

class Plotter(object):
	"""docstring for Plotter"""
	def __init__(self):
		super(Plotter, self).__init__()
		self.gnuplot = Popen(['gnuplot'], stdin=PIPE)

	def close(self):
		self.gnuplot.communicate('quit\n')

	def plot(self, data, save=None, xlabel=None, ylabel=None):
		self._create_data_file(data)
		if save is not None:
			if save[-3:] == 'png':
				self.gnuplot.stdin.write('set terminal png\n')
			elif save[-3:] == 'tex':
				self.gnuplot.stdin.write('set terminal epslatex color\n')
			else:
				self.gnuplot.stdin.write('set terminal %s\n' % save[-3:])
			self.gnuplot.stdin.write('set output "%s"\n' % save)
		if xlabel is not None:
			self.gnuplot.stdin.write('set xlabel "%s"\n' % xlabel)
		if ylabel is not None:
			self.gnuplot.stdin.write('set ylabel "%s"\n' % ylabel)
		self.gnuplot.stdin.write('set style fill solid\n')
		self.gnuplot.stdin.write('plot "data.file" using 2:xtic(1) with histogram\n')
		#self.gnuplot.stdin.write('plot "data.file" using 2:xtic(1) with linespoints\n')

	def _create_data_file(self, data):
		datfile = open('data.file', 'w')
		for (commit, value) in data:
			datfile.write("%s %0.3f\n" % (commit[:5], value))
		datfile.close()

class Tester(object):
	"""docstring for Tester"""
	def __init__(self, original_program, binary=False, input_file=None):
		super(Tester, self).__init__()
		self.original_program = original_program
		self.binary = binary
		self.input_file = input_file
		self._generate_output()

	def _generate_output(self):
		Debugger.log('Generating test output...')
		output_file = open('/tmp/output_original.out', 'wb' if self.binary else 'w')
		input_file = None
		self.original_program.set_output_stream(output_file)
		if self.input_file:
			input_file = open(self.input_file, 'rb' if self.binary else 'r')
			self.original_program.set_input_stream(input_file)
		try:
			self.original_program.run();
		except CalledProcessError:
			Debugger.error('Failed to generate output for ' +
				self.original_program.get_exec_name())
		if self.input_file:
			input_file.close()
		output_file.close()
		Debugger.success()

	def test(self, program):
		Debugger.log('Testing %s...' % program.get_exec_name())
		output_file = open('/tmp/output_tested.out', 'wb' if self.binary else 'w')
		input_file = None
		program.set_output_stream(output_file)
		if self.input_file:
			input_file = open(self.input_file, 'rb' if self.binary else 'r')
			program.set_input_stream(input_file)
		try:
			program.run();
		except CalledProcessError:
			Debugger.error('Failed to generate output for ' +
				program.get_exec_name())
		if self.input_file:
			input_file.close()
		output_file.close()
		if self.binary:
			run(['cmp', '-s', '/tmp/output_original.out', '/tmp/output_tested.out'])
		else:
			run(['diff', '-q', '/tmp/output_original.out', '/tmp/output_tested.out'])
		Debugger.success()


if __name__ == "__main__":
	# import argparse
	# argument_parser = argparse.ArgumentParser(prog=__file__)
	# argument_parser.add_argument('-f', '--file', 
	# 	help='Code to profile. Defaults to main.c', required=True)
	# args = vars(argument_parser.parse_args(sys.argv[1:]))
	# compiler  = Compiler(args.get('file'))
	# program   = compiler.compile('popul')
	# accounter = Accounter(program, 2)
	# try:
	# 	from subprocess import DEVNULL
	# except ImportError:
	# 	from os import devnull
	# 	DEVNULL = open(devnull, 'wb')
	# accounter.account(DEVNULL)
	# accounter.to_CSV()
	# accounter.plot()
	import argparse
	argument_parser = argparse.ArgumentParser(prog=__file__,
		description="Script to load other versions of a given file and "
					"account all of them")
	argument_parser.add_argument('-f', '--file',
		help='File to compare', required=True)
	argument_parser.add_argument('-t', '--testing', action='store_true',
		help='Enable output checking')
	argument_parser.add_argument('-u', '--uncommitted', metavar='FILE',
		help='Add uncommitted files', nargs='+', default=[])
	args = vars(argument_parser.parse_args(sys.argv[1:]))

	# PARAMETERS
	COMPILATION_FLAGS = ['-O3', '-march=native']
	BINARY_OUTPUT = True
	PROGRAM_ARGUMENTS = ['5000']
	INPUT_FILE = 'Makefile'
	# INPUT_FILE = None
	EXT = 'png'

	def acc(commit, code, test=None):
		compiler  = Compiler(code, flags=COMPILATION_FLAGS, from_stdin=True)
		program   = compiler.compile('tmp_'+commit[:5])
		program.set_arguments(PROGRAM_ARGUMENTS)
		if test is not None:
			try:
				test(program)
			except CalledProcessError:
				Debugger.error('Commit %s not producing same output' % commit)
		program.set_output_stream(DEVNULL)
		accounter = Accounter_B(program, 5)
		accounter.account()
		measures  = accounter.get_measures('Elapsed')
		return sum([a['Elapsed'] for a in measures]) / len(measures)

	def acc_with_input(commit, code, test=None):
		compiler  = Compiler(code, flags=COMPILATION_FLAGS, from_stdin=True)
		program   = compiler.compile('tmp_'+commit[:5])
		program.set_arguments(PROGRAM_ARGUMENTS)
		input_file = open(INPUT_FILE, 'rb')
		program.set_input_stream(input_file)
		if test is not None:
			try:
				test(program)
			except CalledProcessError:
				Debugger.error('Commit %s not producing same output' % commit)
		program.set_output_stream(DEVNULL)
		input_file.seek(0)
		# input_file.close()
		# input_file = open(INPUT_FILE, 'rb')
		program.set_input_stream(input_file)
		accounter = Accounter_B(program, 5)
		accounter.account()
		measures  = accounter.get_measures('Elapsed')
		input_file.close()
		return sum([a['Elapsed'] for a in measures]) / len(measures)

	from os import devnull
	DEVNULL = open(devnull, 'wb')
	versioner = Versioner(args.get('file'))
	versions = versioner.get_versions()
	for unc in args.get('uncommitted'):
		code_file = open(unc, 'r')
		versions.append(('uncommitted', code_file.read()))
		code_file.close()
	fun = None
	if args.get('testing'):
		compiler = Compiler(versions[0][1], flags=COMPILATION_FLAGS, from_stdin=True)
		program  = compiler.compile('tmp_test')
		program.set_arguments(PROGRAM_ARGUMENTS)
		tester   = Tester(program, binary=BINARY_OUTPUT, input_file=INPUT_FILE)
		fun = tester.test
	account = acc if INPUT_FILE is None else acc_with_input
	elapseds = [(commit, account(commit, code, fun)) for (commit, code) in versions]
	plotter  = Plotter()
	plotter.plot(elapseds, save='elapseds.'+EXT, xlabel='commit', ylabel='Elapsed Time')
	plotter.close()
	plotter  = Plotter()
	plotter.plot(map(lambda a: (a[0], elapseds[0][1]/a[1]), elapseds),
		save='speedups.'+EXT, xlabel='commit', ylabel='Speed Up')
	for (commit, time) in elapseds:
		print "%s %f\n" % (commit, time)
	DEVNULL.close()