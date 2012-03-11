make clean
make 
sudo mc1322x-load.pl -f xinu.bin -t /dev/ttyUSB1 -c 'bbmc -l redbee-econotag reset'
