all: pq_b.func.demo

pq_b.func.demo: pq_b.o pq_b.func.tester utility.o
	gcc -o pq_b.func.demo pq_b.o pq_b.func.tester utility.o -g

pq_b.perf.demo: pq_b.o pq_b.perf.tester
	gcc -o pq_b.perf.demo pq_b.o pq_b.perf.tester -g

pq_b.o: pq_b.c
	gcc -c -o pq_b.o pq_b.c -g

pq_b.func.tester: pq_b.func.tester.c
	gcc -c -o pq_b.func.tester  pq_b.func.tester.c -g

utility.o: utility.c
	gcc -c -o utility.o utility.c -g

pq_b.perf.tester: pq_b.perf.tester.c
	gcc -c -o pq_b.perf.tester pq_b.perf.tester.c -g

clean:
	rm -f *.demo *.tester *.o
