for _ in {1..100}; do { time ./gal.out dot_graphs/node18.dot c.dot 16 -h dot_graphs/node18.constr; } 2<&1 | grep real | cut -d'm' -f2 | cut -d's' -f1 | tr ',' '.'; done > node18_measures_h.log
