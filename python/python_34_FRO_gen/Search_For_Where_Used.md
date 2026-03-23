## How to find where used.

1. Search for RXI-% which will show the BOM of any model.
It shows
RXI-10FAW-0000
RXI-10N4W-0101C
..
RXI-12NAW-0000


2. Choose the appropriate BOM
For example i choose:
RXI-12NAW-0000

click on it.

3. It shows its BOM:
Components	        Description
--------------------------------------------
317-23184-0001 -  	Kit, Comm, RXI, 1:1
317-23185-0102 -        Kit, RXI, 10MHz Ref, G2, 2Ref


Since 317-23184-0001: Kit, Comm, RXI, 1:1 is appropriate, click on it.
Components	        Description
--------------------------------------------
335-22440-0001 - PCA, IBR, Controller, BT
335-23159-0001 - PCA, IBR, Cntrlr, ARM, ENC, DRO, Org 
335-23165-0001 - PCA, RX1+1, Interface, 50 Ohms

Now we select: 335-23159-0001 since it is part of: RXI-12NAW-0000->317-23184-0001->335-23159-0001 and its the ARM controller not the BT(Bottom) controller.


4. Now Copy 335-23159-0001 in search and find out "Where used"

Components	        Description
--------------------------------------------
317-23216-0011 	Kit,Common,PAA,10V
317-23184-0001 	Kit,Comm,RXI,1:1
...
...

Now out of these, only 317-23184-0001 is appropriate. its like going back in the tree

5. Now Copy: 317-23184-0001 and run where used.

Components	        Description
--------------------------------------------
RXI-10FAW-0000 	RXI,1:1,F,F,N,AC,W
RXI-10NAW-0000 	RXI,1:1,N,N,N,AC,W
RXI-12FAW-0000 	RXI,1:1,2 Ref,F,F,N,AC,W
RXI-12N4W-0000 	RXI,1:1,2 Ref,N,N,N,48V,W
RXI-12NAW-0000 	RXI,1:1,2 Ref,N,N,N,AC,W

Which are all the RXI models and are correct. So we are good to use 

317-23184-0001: Kit, Comm, RXI, 1:1 as the common kit.



