while (<>) { $fuel = int($_ /3) - 2; $ff = $fuel; while (int($ff / 3) - 2 > 0) { $ff = int($ff / 3) - 2; $fuel += $ff; } $sum += $fuel } print $sum, "\n"
