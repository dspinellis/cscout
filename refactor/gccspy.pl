# Character switches that take arguments
$opt_arg{'-D'} = 1;
$opt_arg{'-U'} = 1;
$opt_arg{'-o'} = 1;
$opt_arg{'-e'} = 1;
$opt_arg{'-T'} = 1;
$opt_arg{'-u'} = 1;
$opt_arg{'-I'} = 1;
$opt_arg{'-m'} = 1;
$opt_arg{'-x'} = 1;
$opt_arg{'-L'} = 1;
$opt_arg{'-A'} = 1;
$opt_arg{'-V'} = 1;
$opt_arg{'-B'} = 1;
$opt_arg{'-b'} = 1;

# Word switches that take arguments
$opt_arg{'-Tdata'} = 1;
$opt_arg{'-Ttext'} = 1;
$opt_arg{'-Tbss'} = 1;
$opt_arg{'-include'} = 1;
$opt_arg{'-imacros'} = 1;
$opt_arg{'-aux-info'} = 1;
$opt_arg{'-idirafter'} = 1;
$opt_arg{'-iprefix'} = 1;
$opt_arg{'-iwithprefix'} = 1;
$opt_arg{'-iwithprefixbefore'} = 1;
$opt_arg{'-isystem'} = 1;
$opt_arg{'--param'} = 1;
$opt_arg{'-specs'} = 1;
$opt_arg{'-MF'} = 1;
$opt_arg{'-MT'} = 1;
$opt_arg{'-MQ'} = 1;
$opt_arg{'-Xlinker'} = 1;

# Switches that don't proceed with linking
$no_link{'c'} = 1;
$no_link{'S'} = 1;


# Long to short mapping, should be scanned sequentially
# arg value:
#    a: argument required.
#    o: argument optional.
#    j: join argument to equivalent, making one word.
#    *: require other text after NAME as an argument.

push(@map, {long => '--all-warnings', short => '-Wall', arg => 0});
push(@map, {long => '--ansi', short => '-ansi', arg => 0});
push(@map, {long => '--assemble', short => '-S', arg => 0});
push(@map, {long => '--assert', short => '-A', arg => 'a'});
push(@map, {long => '--classpath', short => '-fclasspath=', arg => 'aj'});
push(@map, {long => '--bootclasspath', short => '-fbootclasspath=', arg => 'aj'});
push(@map, {long => '--CLASSPATH', short => '-fclasspath=', arg => 'aj'});
push(@map, {long => '--comments', short => '-C', arg => 0});
push(@map, {long => '--comments-in-macros', short => '-CC', arg => 0});
push(@map, {long => '--compile', short => '-c', arg => 0});
push(@map, {long => '--debug', short => '-g', arg => 'oj'});
push(@map, {long => '--define-macro', short => '-D', arg => 'aj'});
push(@map, {long => '--dependencies', short => '-M', arg => 0});
push(@map, {long => '--dump', short => '-d', arg => 'a'});
push(@map, {long => '--dumpbase', short => '-dumpbase', arg => 'a'});
push(@map, {long => '--entry', short => '-e', arg => 0});
push(@map, {long => '--extra-warnings', short => '-W', arg => 0});
push(@map, {long => '--for-assembler', short => '-Wa', arg => 'a'});
push(@map, {long => '--for-linker', short => '-Xlinker', arg => 'a'});
push(@map, {long => '--force-link', short => '-u', arg => 'a'});
push(@map, {long => '--imacros', short => '-imacros', arg => 'a'});
push(@map, {long => '--include', short => '-include', arg => 'a'});
push(@map, {long => '--include-barrier', short => '-I-', arg => 0});
push(@map, {long => '--include-directory', short => '-I', arg => 'aj'});
push(@map, {long => '--include-directory-after', short => '-idirafter', arg => 'a'});
push(@map, {long => '--include-prefix', short => '-iprefix', arg => 'a'});
push(@map, {long => '--include-with-prefix', short => '-iwithprefix', arg => 'a'});
push(@map, {long => '--include-with-prefix-before', short => '-iwithprefixbefore', arg => 'a'});
push(@map, {long => '--include-with-prefix-after', short => '-iwithprefix', arg => 'a'});
push(@map, {long => '--language', short => '-x', arg => 'a'});
push(@map, {long => '--library-directory', short => '-L', arg => 'a'});
push(@map, {long => '--machine', short => '-m', arg => 'aj'});
push(@map, {long => '--machine-', short => '-m', arg => '*j'});
push(@map, {long => '--no-integrated-cpp', short => '-no-integrated-cpp', arg => 0});
push(@map, {long => '--no-line-commands', short => '-P', arg => 0});
push(@map, {long => '--no-precompiled-includes', short => '-noprecomp', arg => 0});
push(@map, {long => '--no-standard-includes', short => '-nostdinc', arg => 0});
push(@map, {long => '--no-standard-libraries', short => '-nostdlib', arg => 0});
push(@map, {long => '--no-warnings', short => '-w', arg => 0});
push(@map, {long => '--optimize', short => '-O', arg => 'oj'});
push(@map, {long => '--output', short => '-o', arg => 'a'});
push(@map, {long => '--output-class-directory', short => '-foutput-class-dir=', arg => 'ja'});
push(@map, {long => '--param', short => '--param', arg => 'a'});
push(@map, {long => '--pedantic', short => '-pedantic', arg => 0});
push(@map, {long => '--pedantic-errors', short => '-pedantic-errors', arg => 0});
push(@map, {long => '--pie', short => '-pie', arg => 0});
push(@map, {long => '--pipe', short => '-pipe', arg => 0});
push(@map, {long => '--prefix', short => '-B', arg => 'a'});
push(@map, {long => '--preprocess', short => '-E', arg => 0});
push(@map, {long => '--print-search-dirs', short => '-print-search-dirs', arg => 0});
push(@map, {long => '--print-file-name', short => '-print-file-name=', arg => 'aj'});
push(@map, {long => '--print-libgcc-file-name', short => '-print-libgcc-file-name', arg => 0});
push(@map, {long => '--print-missing-file-dependencies', short => '-MG', arg => 0});
push(@map, {long => '--print-multi-lib', short => '-print-multi-lib', arg => 0});
push(@map, {long => '--print-multi-directory', short => '-print-multi-directory', arg => 0});
push(@map, {long => '--print-multi-os-directory', short => '-print-multi-os-directory', arg => 0});
push(@map, {long => '--print-prog-name', short => '-print-prog-name=', arg => 'aj'});
push(@map, {long => '--profile', short => '-p', arg => 0});
push(@map, {long => '--profile-blocks', short => '-a', arg => 0});
push(@map, {long => '--quiet', short => '-q', arg => 0});
push(@map, {long => '--resource', short => '-fcompile-resource=', arg => 'aj'});
push(@map, {long => '--save-temps', short => '-save-temps', arg => 0});
push(@map, {long => '--shared', short => '-shared', arg => 0});
push(@map, {long => '--silent', short => '-q', arg => 0});
push(@map, {long => '--specs', short => '-specs=', arg => 'aj'});
push(@map, {long => '--static', short => '-static', arg => 0});
push(@map, {long => '--std', short => '-std=', arg => 'aj'});
push(@map, {long => '--symbolic', short => '-symbolic', arg => 0});
push(@map, {long => '--time', short => '-time', arg => 0});
push(@map, {long => '--trace-includes', short => '-H', arg => 0});
push(@map, {long => '--traditional', short => '-traditional', arg => 0});
push(@map, {long => '--traditional-cpp', short => '-traditional-cpp', arg => 0});
push(@map, {long => '--trigraphs', short => '-trigraphs', arg => 0});
push(@map, {long => '--undefine-macro', short => '-U', arg => 'aj'});
push(@map, {long => '--user-dependencies', short => '-MM', arg => 0});
push(@map, {long => '--verbose', short => '-v', arg => 0});
push(@map, {long => '--warn-', short => '-W', arg => '*j'});
push(@map, {long => '--write-dependencies', short => '-MD', arg => 0});
push(@map, {long => '--write-user-dependencies', short => '-MMD', arg => 0});
push(@map, {long => '--', short => '-f', arg => '*j'});

# Translate long options
for ($i = 0; $i <= $#ARGV; $i++) {
	$opt = $ARGV[$i];
	if ($opt =~ m/^\-\-.*/) {
		# Long option: translate
		mapcheck:
		for ($j = 0; $j <= $#map; $j++) {
			if ($opt =~ m/^$map[$j]{long}/) {
				undef $optarg;
				if (length($opt > length($map[$j]{long}) {
					if ($opt ~= m/\=(.*)$/) {
						$optarg = $1;
					} elsif ($map[$j]{arg} =~ m/\*/) {
						$optarg = substr($opt, length($map[$j]{long}));
					} else {
						next mapcheck;
					}
				}
				if ($map[$j]{arg} =~ m/a/) {
					$optarg = $ARGV[++$i] unless ($optarg);
				} elsif ($map[$j]{arg} =~ m/\*/) {
					;
				} elsif ($map[$j]{arg} !~ m/o/) {
					undef($optarg);
				}
				if ($optarg) {
					if ($map[$j]{arg} =~ m/j/) {
						$ARGV2[$ni++] = $map[$j]{short} . $optarg;
					} else {
						$ARGV2[$ni++] = $map[$j]{short};
						$ARGV2[$ni++] = $optarg;
					}
				} else {
					$ARGV2[$ni++] = $map[$j]{short};
				}
				last mapcheck;
			}
		}
	} elsif ($opt =~ m/^\-.*/) {
		# Short option
		$ARGV2[$ni++] = $opt;
		$ARGV2[$ni++] = $ARGV[++$i] if ($opt_arg{$opt});
		push(@libs, $1);
		# Operands and +e options
		$ARGV2[$ni++] = $opt;
		push(@libs, $ARGV[++$i]);
		push(@afiles, $arg);
	} else {
# Handle the following preprocessor options
#             -C -dD -dM -dN -Dmacro[=defn] -E -H -idirafter dir
#             -include file -imacros file -iprefix file -iwithprefix dir -M
#             -MD -MM -MMD -nostdinc -P -Umacro -undef
	if ($opt eq '-include') {
		$process .= "#pragma process $opt_arg\n";
	} elsif ($opt eq '-imacros') {
		$process .= "#include $opt_arg\n";
	} elsif ($opt eq '-idirafter') {
		$idir2 .= "#pragma includepath $opt_arg\n";
	} elsif ($opt eq '-idirafter') {
		$idir2 .= "#pragma includepath $opt_arg\n";
	} elsif ($opt eq '-iprefix') {
		$iprefix = $opt_arg;
	} elsif ($opt eq '-iwithprefix') {
		$idir2 .= "#pragma includepath $prefix/$opt_arg\n";
	} elsif ($opt eq '-iwithprefixbefore') {
		$idir2 = "#pragma includepath $prefix/$opt_arg\n" .$idir2;
		$idir2 .= "#pragma includepath $prefix/$opt_arg\n";
	$preprocess = 1 if ($arg eq '--preprocess' || $arg eq '-E');
	while (<IN>) {
$idir2
$process
