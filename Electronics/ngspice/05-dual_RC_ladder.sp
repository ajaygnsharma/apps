.title dual rc ladder
R1 int in 10k
V1 in 0 dc 0 PULSE (0 5 1u 1u 1u 1 1)
R2 out int 1k
C1 int 0 1u
C2 out 0 100n

.control
tran 50u 50m
plot in int out
.endc

.end

