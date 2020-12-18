#!/usr/bin/env perl

use strict;
use warnings;

my $saw_include;
my $added_namespace_start;
my $added_namespace_end;

# Use of this script was:
# perl -i add_namespace.pl `find . -name '*.h' -o -name '*.cc'`

# TODO(@monkeynova): There were manual fixups after running this
# 1. ABSL_FLAG needs to be in the global namespace (otherwise it creates an
#    absl namespace inside the local namespace and absl::Foo gets conflicts).
# 2. Header files with no #include directives didn't add the namespace.
# 3. Point.h put the ending namespace after the #endif directive. This was due
#    to an extraneous space between '//' and 'ADVENT_OF_CODE...'.
# 4. 2019/day25_interactive.cc (which contains its own main) shouldn't have put
#    main in a namespace.


while (<>) {
  if (eof) {
      undef $saw_include;
      undef $added_namespace_start;
      undef $added_namespace_end;
  }
  if (/#include/) { $saw_include = 1; }

  if (/^[^#\s]/ && $saw_include && !$added_namespace_start) {
    $added_namespace_start = 1;
    if ($ARGV =~ /day\d+\.cc/) {
      print "namespace advent_of_code {\nnamespace {\n\n";
    } else {
      print "namespace advent_of_code {\n\n";
    }
  }

  if ($ARGV =~ /day\d+\.cc/ && /::Part1\(/) {
    print "}  // namespace\n\n";
  }

  if (m{#endif  // ADVENT_OF_CODE}) {
    print "}  // namespace advent_of_code\n\n";
    $added_namespace_end = 1;
  }
  print;
  if (eof && !$added_namespace_end) {
    print "\n}  // namespace advent_of_code\n";
  }
}

