10 'ENV.IN Test Program
20 ENV.IN [0]
30 IF [4]<>0 THEN ? "ERR";[4]:GOTO 20
40 ? "T=";[0]/10;".";[0]%10;"C"
50 ? "H=";[1]/10;".";[1]%10;"%"
60 IF [5]=3 THEN P=[2]+[3]*256:? "P=";P;"Pa"
70 WAIT 60
80 GOTO 20